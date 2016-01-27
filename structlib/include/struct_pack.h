#pragma once

#include "exceptions.h"
#include "std_vector.h"

#define _USE_STDC_MALLOC_

namespace FWutils {
	/**
	Structure packer class
	Assebles a series of record from individual vector of its fields. 
	*/
	class StructPack
	{
	public:
		StructPack();

		/// Set padding between fields in byte. Should be 2^n. 
		void setFieldPadding(size_t padding) { this->m_field_padding = padding; }
		
		/// Set the padding/alignment between two records. Should be 2^n.  Default: 1
		void setRecordPadding(size_t padding) { this->m_record_align = padding; }

		/**
		Adds a field 
		@return id
		*/
		int addField(size_t width);

		/**
		Assigns a data source to the record descriptor
		*/
		void addPointer(int id, const void *pointer, size_t offset, size_t stride);

		/**
		Assebles the structure 
		@param count Count of records
		@return the created buffer pointer
		*/
		void* operator() (size_t count);
		
		//-- getters 

		void* getBuffer() { return this->m_buffer; }		///< @return the created buffer pointer. Has to be freed outside the class.
		size_t getCount() { return m_record_count; }		///< @return the count of records
		size_t getRecordWidth() { return m_record_width; }	///< @retturn size of a record
		size_t getBufferSize() { return m_record_count * m_record_width; } ///< @return entire buffer size

	private:
		/// Calculates sizes and fills internal offsets 
		void calcSize();

	protected:
		/**
		@{
		Allocation procedure is hidden in a member function due overriding it
		*/
		virtual void* alloc(size_t size);
		virtual void free (void* ptr);
		virtual void zeroMemory(void* ptr, size_t size);
		/**
		@}
		*/

	protected:
		unsigned char *m_buffer;

		size_t m_record_align;
		size_t m_field_padding;
		size_t m_record_width;
		size_t m_record_count;
		
		struct struct_field_t {
			int id;
			size_t out_offset;
			size_t out_width;

			const void* in_ptr;
			size_t in_offset;
			size_t in_stride;
		};

		FWutils::vector<struct_field_t > m_elems;
	};
}

DEFINE_EXCEPTION(InvalidIDException, 0, "Invlid index or id exception");
DEFINE_EXCEPTION(ZeroOffsetException, 0, "Zero offset, byte width or offset exception");