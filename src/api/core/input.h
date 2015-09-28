////////////////////////////////////////////////////////////////////////////////
// Filename: Input.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Input_H_
#define _Input_H_

namespace FWcore {
	class Input
	{
	public:
		Input();
		~Input();

		void Initialize();

		void KeyDown(unsigned int);
		void KeyUp(unsigned int);

		bool IsKeyDown(unsigned int);

	private:
		bool m_keys[256];
	};

}
#endif