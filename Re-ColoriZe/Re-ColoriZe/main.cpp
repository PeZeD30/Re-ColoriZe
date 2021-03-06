//Installer le windows 10 SDK (10.0.17763.132) pour avoir le header Xinput
//inclure xinput.lib au proprietes du projet 

#include <SFML/Graphics.hpp>
#include "Controller.h"
#include <iostream>
#include <conio.h>
#include <vector>
#include <math.h>

#define VIBRATION_DELAY 0.2f

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
	window.setFramerateLimit(60);


	//create a new controller here it will be number 1
	Controller* controller = new Controller(1);

	sf::CircleShape testCircle = sf::CircleShape(20);
	testCircle.setOrigin(sf::Vector2f(20, 20));
	testCircle.setPosition(sf::Vector2f(400, 400));

	sf::Clock vibrationTimer;
	vibrationTimer.restart();

	bool isVibrating = false;

	//variable to store vibration power for both left and right motor (between 0-65535)
	int lpower = 10000;
	int rpower = 10000;

	std::cout << "A vibration courte des 2 cotes" << std::endl;
	std::cout << "Y vibration continue droite" << std::endl;
	std::cout << "X vibration continue gauche" << std::endl;
	std::cout << "B couper vibration" << std::endl;
	std::cout << "Stick gauche deplacer le cercle" << std::endl;
	std::cout << "START quitter" << std::endl;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		
		if (controller->isConnected())
		{
			//std::cout << "connected" << std::endl;
			//if button A is pressed
			if (controller->getState().Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				controller->vibrate(lpower, rpower);
				isVibrating = true;
				vibrationTimer.restart();
			}
			//if button B is pressed
			if (controller->getState().Gamepad.wButtons & XINPUT_GAMEPAD_B) {
				controller->vibrate();
			}
			//if button Y is pressed
			if (controller->getState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
				controller->vibrate(0, rpower);
			}
			//if button X is pressed
			if (controller->getState().Gamepad.wButtons & XINPUT_GAMEPAD_X) {
				controller->vibrate(lpower, 0);
			}
			//if button START is pressed
			if (controller->getState().Gamepad.wButtons & XINPUT_GAMEPAD_START) {
				window.close();
			}

			//get Leftthumbstick value (between -32767 and +32767) for both X and Y axis
			float LX = controller->getState().Gamepad.sThumbLX;
			float LY = controller->getState().Gamepad.sThumbLY;


			///if right stick just change by theses two lines
			//float LX = controller->getState().Gamepad.sThumbRX;
			//float LY = controller->getState().Gamepad.sThumbRY;


			//determine how far the controller is pushed
			float magnitude = sqrt(LX*LX + LY * LY);

			//determine the direction the controller is pushed
			float normalizedLX = LX / magnitude;
			float normalizedLY = LY / magnitude;

			//variable to store the normalized magnitude (between 0 and 1)
			float normalizedMagnitude = 0;

			//check if the left thumbstick is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				//readjust the magnitude if its higher than the max axis value
				if (magnitude > 32767)
				{
					magnitude = 32767;
				}

				//adjust magnitude rremoving the deadzone value (now magnitude is relative to the start of the deadzone end)
				magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				//normalize the magnitude to get a value between 0 and 1
				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			}
			//if the left thumbstick is inside the deadzone
			else
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
			}

			//std::cout << "Normalized dir X : " << normalizedLX * normalizedMagnitude << " , dir Y : " << normalizedLY * normalizedMagnitude << std::endl;

			//store the direction to move by multiplying the axis value by the normalizedMagnitude
			sf::Vector2f actualDir;
			actualDir.x = normalizedLX * normalizedMagnitude;
			actualDir.y = (-normalizedLY) * normalizedMagnitude;

			//std::cout << "actual dir" << actualDir.x << std::endl;

			//set the circle position with the calculated dir
			testCircle.setPosition(sf::Vector2f(testCircle.getPosition().x + (actualDir.x), testCircle.getPosition().y + (actualDir.y)));


		}
		else
		{
			//std::cout << "not connected" << std::endl;
		}



		if (isVibrating)
		{
			if (vibrationTimer.getElapsedTime().asSeconds() > VIBRATION_DELAY)
			{
				controller->vibrate();
				isVibrating = false;
			}
		}

		window.draw(testCircle);

		window.display();
	}

	return 0;
}