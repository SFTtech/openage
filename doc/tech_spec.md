* Original Age of Empires Graphics/Sounds/Resources:
	* Are extracted by the setup script (Python) and converted to a directory structure in a suitable format (TBD)

* Build system:
	* `cmake`

* Application:
	* Client:
		* C++, using SDL2. Rendering via OpenGL 2.1 incl shaders
		* Audio via SDL2
		* Networking via SDL2
		* No threading (for the beginning)
	* Server:
		* C++, very minimal
		* Lobby server + player action relay (does not run the game simulation)
	* Launcher:
		* Python, GTK3
		* gui for connecting to lobby server

* C++ coding styles:
	* let the flame wars begin; see subdirectory 'code_style'

* Code documentation: Doxygen
