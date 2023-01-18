/** A quick and dirty OOP window-thing for imgui*/

#pragma once

namespace UI {

	/**
	common stuff to handling windows
	*/

	class Window {
	public:
		Window();
		virtual ~Window();

		virtual void Render() = 0;
	};

}
