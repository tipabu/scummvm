/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "asylum/menu.h"
#include "asylum/resourcepack.h"
#include "asylum/graphics.h"

namespace Asylum {

/** This fixes the menu icons text x position on screen */
const int MenuIconFixedXpos[12] = { 28, 128, 225, 320, 410, 528, 16, 115, 237, 310, 508, 419 };

MainMenu::MainMenu(Screen *screen, Sound *sound): _screen(screen), _sound(sound) {
	_mouseX             = 0;
	_mouseY             = 0;
	_leftClick           = false;
	_activeIcon         = -1;
	_previousActiveIcon = -1;
	_curIconFrame       = 0;
	_curMouseCursor     = 0;
	_cursorStep         = 1;
	_activeMenuScreen   = kMainMenu;

	_resPack = new ResourcePack(1);

	// Load the graphics palette
	_screen->setPalette(_resPack, 17);

	// Initialize background
	_bgResource = new GraphicResource(_resPack, 0);
	// Copy the bright background to the back buffer
	GraphicFrame *bg = _bgResource->getFrame(1);
	_screen->copyToBackBuffer((byte *)bg->surface.pixels, 0, 0, bg->surface.w, bg->surface.h);

	// Initialize eye animation
	_eyeResource = new GraphicResource(_resPack, 1);

	// Initialize mouse cursor
	_cursorResource = new GraphicResource(_resPack, 2);
	_screen->setCursor(_cursorResource, 0);
	_screen->showCursor();

	_iconResource = 0;

	_sound->playMusic(_resPack, 39);

    _text = new Text(_screen);
    _text->loadFont(_resPack, 16);	// 0x80010010, yellow font
	_textBlue = new Text(_screen);
	_textBlue->loadFont(_resPack, 22);	// 0x80010016, blue font
}

MainMenu::~MainMenu() {
	delete _textBlue;
    delete _text;
	delete _iconResource;
	delete _eyeResource;
	delete _cursorResource;
	delete _bgResource;
	delete _resPack;
}

void MainMenu::handleEvent(Common::Event *event, bool doUpdate) {
	_ev = event;

	switch (_ev->type) {
	case Common::EVENT_MOUSEMOVE:
		_mouseX = _ev->mouse.x;
		_mouseY = _ev->mouse.y;
		break;
	case Common::EVENT_LBUTTONUP:
		_leftClick = true;
		break;
	default:
		break;
	}

	if (doUpdate || _leftClick)
		update();
}

void MainMenu::update() {
	updateEyesAnimation();
	updateCursor();

	if (_activeMenuScreen == kMainMenu) {
		updateMainMenu();
	} else {
		updateSubMenu();
	}

	if (_leftClick) {
		_leftClick = false;
		
		if (_activeIcon != -1) {
			// Copy the dark background to the back buffer
			GraphicFrame *bg = _bgResource->getFrame(0);
			_screen->copyToBackBuffer((byte *)bg->surface.pixels, 0, 0, bg->surface.w, bg->surface.h);
			_activeMenuScreen = (MenuScreen) _activeIcon;

			// Set the cursor
			delete _cursorResource;
			_cursorResource = new GraphicResource(_resPack, 3);
			_screen->setCursor(_cursorResource, 0);
		}

		switch (_activeIcon) {
			case kNewGame:
				// TODO
				break;
			case kLoadGame:
				// TODO
				break;
			case kSaveGame:
				// TODO
				break;
			case kDeleteGame:
				// TODO
				break;
			case kViewCinematics:
				// TODO
				break;
			case kQuitGame: 
				// Nothing here
				break;
			case kTextOptions:
				// TODO
				break;
			case kAudioOptions:
				// TODO
				break;
			case kSettings:
				// TODO
				break;
			case kKeyboardConfig:
				// TODO
				break;
			case kShowCredits:
				// TODO
				break;
			case kReturnToGame:
				// TODO
				break;
		}
	}
}

void MainMenu::updateCursor() {
	_curMouseCursor += _cursorStep;
	if (_curMouseCursor == 0)
		_cursorStep = 1;
	if (_curMouseCursor == _cursorResource->getFrameCount() - 1)
		_cursorStep = -1;

	_screen->setCursor(_cursorResource, _curMouseCursor);
}

void MainMenu::updateEyesAnimation() {
	// Eyes animation
	// Get the appropriate eye resource depending on the mouse position
	int eyeFrameNum = kEyesFront;

	if (_mouseX <= 200) {
		if (_mouseY <= 160)
			eyeFrameNum = kEyesTopLeft;
		else if (_mouseY > 160 && _mouseY <= 320)
			eyeFrameNum = kEyesLeft;
		else
			eyeFrameNum = kEyesBottomLeft;
	} else if (_mouseX > 200 && _mouseX <= 400) {
		if (_mouseY <= 160)
			eyeFrameNum = kEyesTop;
		else if (_mouseY > 160 && _mouseY <= 320)
			eyeFrameNum = kEyesFront;
		else
			eyeFrameNum = kEyesBottom;
	} else if (_mouseX > 400) {
		if (_mouseY <= 160)
			eyeFrameNum = kEyesTopRight;
		else if (_mouseY > 160 && _mouseY <= 320)
			eyeFrameNum = kEyesRight;
		else
			eyeFrameNum = kEyesBottomRight;
	}
	// TODO: kEyesCrossed state

	GraphicFrame *eyeFrame = _eyeResource->getFrame(eyeFrameNum);
	_screen->copyRectToScreenWithTransparency((byte *)eyeFrame->surface.pixels, eyeFrame->x, eyeFrame->y, eyeFrame->surface.w, eyeFrame->surface.h);
}

void MainMenu::updateMainMenu() {
	int rowId = 0;

	if (_mouseY >= 20 && _mouseY <= 20 + 48) {
		rowId = 0; // Top row
	} else if (_mouseY >= 400 && _mouseY <= 400 + 48) {
		rowId = 1; // Bottom row
	} else {
		// No row selected
		_previousActiveIcon = _activeIcon = -1;
		_leftClick = false;
		return;
	}

	// Icon animation
	for (int i = 0; i <= 5; i++) {
		int curX = 40 + i * 100;
		if (_mouseX >= curX && _mouseX <= curX + 55) {
			int iconNum = i + 6 * rowId;
			_activeIcon = iconNum;

			// The last 2 icons are swapped
			if (iconNum == 11)
				iconNum = 10;
			else if (iconNum == 10)
				iconNum = 11;

			// Get the current icon animation
			if (!_iconResource || _activeIcon != _previousActiveIcon) {
				delete _iconResource;
				_iconResource = new GraphicResource(_resPack, iconNum + 4);
			}

			GraphicFrame *iconFrame = _iconResource->getFrame(MIN<int>(_iconResource->getFrameCount() - 1, _curIconFrame));
			_screen->copyRectToScreenWithTransparency((byte *)iconFrame->surface.pixels, iconFrame->x, iconFrame->y, iconFrame->surface.w, iconFrame->surface.h);

			// Cycle icon frame
			_curIconFrame++;
			if (_curIconFrame >= _iconResource->getFrameCount())
				_curIconFrame = 0;

			// Show text
            _text->drawResTextCentered(MenuIconFixedXpos[iconNum], iconFrame->y + 50, _text->getResTextWidth(iconNum + 1309), iconNum + 1309);
			
			// Play creepy voice
			if (!_sound->isSfxActive() && _activeIcon != _previousActiveIcon) {
				_sound->playSfx(_resPack, iconNum + 44);
				_previousActiveIcon = _activeIcon;
			}

			break;
		}
	}
}

void MainMenu::updateSubMenu() {
	GraphicFrame *iconFrame = _iconResource->getFrame(MIN<int>(_iconResource->getFrameCount() - 1, _curIconFrame));
	_screen->copyRectToScreenWithTransparency((byte *)iconFrame->surface.pixels, iconFrame->x, iconFrame->y, iconFrame->surface.w, iconFrame->surface.h);

	// Cycle icon frame
	_curIconFrame++;
	if (_curIconFrame >= _iconResource->getFrameCount())
		_curIconFrame = 0;

	switch (_activeIcon) {
		case kNewGame:
            updateSubMenuNewGame();
			break;
		case kLoadGame:
			// TODO
			break;
		case kSaveGame:
			// TODO
			break;
		case kDeleteGame:
			// TODO
			break;
		case kViewCinematics:
			// TODO
			break;
		case kQuitGame:
			updateSubMenuQuitGame();
			break;
		case kTextOptions:
			// TODO
			break;
		case kAudioOptions:
			// TODO
			break;
		case kSettings:
			// TODO
			break;
		case kKeyboardConfig:
			// TODO
			break;
		case kShowCredits:
			// TODO
			break;
		case kReturnToGame:
			// TODO
			break;
	}
}

void MainMenu::updateSubMenuNewGame() {
    // begin new game
	_text->drawResTextCentered(10, 100, 620, 1321);	// 0x80000529u

	// Yes
	if (_mouseX >= 240 && _mouseX <= 280 && _mouseY >= 280 && _mouseY <= 300) {
		_textBlue->setTextPos(247, 273);
		_textBlue->drawResText(1322); // 0x8000052Au

		if (_leftClick) {
			_leftClick = false;

			// TODO handle new game event

            // FIXME scene proof-of-concept
            //_gameState = new GameState(_screen, _sound, 5);
		}
	} else {
		_text->setTextPos(247, 273);
		_text->drawResText(1322); // 0x8000052Au
	}

	// No
	if (_mouseX >= 360 && _mouseX <= 400 && _mouseY >= 280 && _mouseY <= 300) {
		_textBlue->setTextPos(369, 273);
		_textBlue->drawResText(1323); // 0x8000052Bu

		if (_leftClick)
			exitSubMenu();
	} else {
		_text->setTextPos(369, 273);
		_text->drawResText(1323); // 0x8000052Bu
	}
}

void MainMenu::updateSubMenuQuitGame() {
    // Quit confirmation
	_text->drawResTextCentered(10, 100, 620, 1408);	// 0x80000580u

	// Yes
	if (_mouseX >= 240 && _mouseX <= 280 && _mouseY >= 280 && _mouseY <= 300) {
		_textBlue->setTextPos(247, 273);
		_textBlue->drawResText(1409); // 0x80000581u

		if (_leftClick) {
			_leftClick = false;

			// User clicked on quit, so push a quit event
			Common::Event event;
			event.type = Common::EVENT_QUIT;
			g_system->getEventManager()->pushEvent(event);
		}
	} else {
		_text->setTextPos(247, 273);
		_text->drawResText(1409); // 0x80000581u
	}

	// No
	if (_mouseX >= 360 && _mouseX <= 400 && _mouseY >= 280 && _mouseY <= 300) {
		_textBlue->setTextPos(369, 273);
		_textBlue->drawResText(1410); // 0x80000582u

		if (_leftClick)
			exitSubMenu();
	} else {
		_text->setTextPos(369, 273);
		_text->drawResText(1410); // 0x80000582u
	}
}

void MainMenu::exitSubMenu() {
	_leftClick = false;
	_activeMenuScreen = kMainMenu;

	// Copy the bright background to the back buffer
	GraphicFrame *bg = _bgResource->getFrame(1);
	_screen->copyToBackBuffer((byte *)bg->surface.pixels, 0, 0, bg->surface.w, bg->surface.h);

	// Set the cursor
	delete _cursorResource;
	_cursorResource = new GraphicResource(_resPack, 2);
	_screen->setCursor(_cursorResource, 0);
}

} // end of namespace Asylum