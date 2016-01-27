#include "struct_pack.h"

using namespace FWdebugExceptions;

FWutils::StructPack::StructPack() : 
	m_buffer(NULL),
	m_field_padding(0),
	m_record_align(1)
{
}

int FWutils::StructPack::addField(size_t width)
{
	if (!width)
		throw EX(ZeroOffsetException);

	int id = this->m_elems.size();
	struct struct_field_t elem;
	this->zeroMemory(&elem, sizeof(elem));

	elem.id = id;
	elem.out_width = width;

	this->m_elems << elem;

	return id;
}

void FWutils::StructPack::addPointer(int id, const void * pointer, size_t offset, size_t stride)
{
	if (m_buffer) 
		throw EX(InvalidOperationException);
	
	if (!pointer) throw EX(NullPointerException);
	if (!stride) throw EX(ZeroOffsetException);

	if (id >= this->m_elems.size()) throw EX(InvalidIDException);

	struct struct_field_t &elem = this->m_elems[id];

	elem.in_ptr = pointer;
	//elem.in_width = size;
	elem.in_offset = offset;
	elem.in_stride = stride;

}

void * FWutils::StructPack::operator()(size_t count)
{
	// + throw if count zero 
	this->m_record_count = count;

	if (this->m_buffer) { 
		this->free(this->m_buffer); 
	}
	
	this->calcSize();
	this->m_buffer = (unsigned char*)this->alloc(this->getBufferSize());
	this->zeroMemory(this->m_buffer, this->getBufferSize());

	for (size_t i = 0; i < this->m_elems.size(); i++)
	{
		struct struct_field_t &elem = this->m_elems[i];

		// start address of the current structure element
		unsigned char* dst_struct = this->m_buffer + elem.out_offset;

		if (!elem.in_ptr)
			continue;
		
		const unsigned char* src_struct = (const unsigned char*)elem.in_ptr + elem.in_offset;

		size_t j = count;
		while (j--) {
			{
				size_t k = elem.out_width;
				const unsigned char *src = src_struct;
				unsigned char *dst = dst_struct;

				while (k--) {
					dst[k] = src[k];
				}
			}

			dst_struct += this->m_record_width;
			src_struct += elem.in_stride;
		}
	}
	
	return this->m_buffer;
}

// ========================================================

void FWutils::StructPack::calcSize()
{
	// @todo check alignment && padding 

	size_t last_offset = 0;
	for (size_t i = 0; i < this->m_elems.size(); i++) {
		struct struct_field_t &elem = this->m_elems[i];
		
		// ez lesz egy field hossza
		size_t elem_width = elem.out_width;

		if (m_field_padding > elem_width)
		{
			elem_width = m_field_padding;
		}
		else if (m_field_padding < elem_width && m_field_padding > 0)
		{
			elem_width = (1 + (elem_width / m_field_padding)) * m_field_padding;
		}

		elem.out_offset = last_offset;
		last_offset += elem_width;
	}

	m_record_width = last_offset;

	if ((m_record_width / m_record_align) * m_record_align < m_record_width) {
		m_record_width = (1+(m_record_width / m_record_align)) * m_record_align;
	}
}

// ========================================================

#ifdef _USE_STDC_MALLOC_
void * FWutils::StructPack::alloc(size_t size)
{
	return malloc(size);
}

void FWutils::StructPack::free(void * ptr)
{
	free(ptr);
}

void FWutils::StructPack::zeroMemory(void * ptr, size_t size)
{
	memset(ptr, 0, size);
}

#endif //_USE_LIBC_ALLOCATOR
