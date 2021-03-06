/*
 * RobotCarGui.cpp
 *
 *  Contains all common GUI elements for operating and controlling the RobotCar.
 *
 *  Calibration: Sets lowest speed for which wheels are moving.
 *  Speed Slider left: Sets speed for manual control which serves also as maximum speed for autonomous drive if "Stored"
 *  Store: Stores calibration info and maximum speed.
 *  Cont ->Step / Step -> SStep, SStep->Cont: Switches mode from "continuous drive" to "drive until next turn" to "drive CENTIMETER_PER_RIDE_PRO"
 *  Start Simple: Start simple driving algorithm (using the 2 "simple" functions in RobotCar.cpp)
 *  Start Pro: Start elaborated driving algorithm
 *
 *  insertToPath() and DrawPath() to show the path we were driving.
 *
 *  Needs BlueDisplay library.
 *
 *  Created on: 20.09.2016
 *  Copyright (C) 2016  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 */

#include "RobotCar.h"
#include "RobotCarGui.h"

BDButton TouchButtonTestPage;
BDButton TouchButtonLaser;
BDButton TouchButtonMelody;
BDButton TouchButtonCameraOnOff;

BDSlider SliderLaserPositionHorizontal;
#ifdef USE_PAN_TILT_SERVO
BDSlider SliderLaserPositionVertical;
#endif

// Here we get values from 0 to 180 degrees from scaled slider
void doLaserServoPosition(BDSlider * aTheTouchedSlider, uint16_t aValue) {
    LaserPanServo.write(aValue);
}

#ifdef USE_PAN_TILT_SERVO
void doVerticalLaserServoPosition(BDSlider * aTheTouchedSlider, uint16_t aValue) {
    LaserTiltServo.write(aValue);
}
#endif

void doLaserOnOff(BDButton * aTheTouchedButton, int16_t aValue) {
    digitalWrite(LASER_OUT_PIN, aValue);
}

#ifdef USE_TB6612_BREAKOUT_BOARD
void doCameraSupplyOnOff(BDButton * aTheTouchedButton, int16_t aValue) {
    digitalWrite(CAMERA_SUPPLY_CONTROL_PIN, aValue);
}
#endif

#ifdef ENABLE_RTTTL
void doPlayMelody(BDButton * aTheTouchedButton, int16_t aValue) {
    sPlayMelody = aValue;
}
#endif

void initHomePage(void) {

    TouchButtonTestPage.init(BUTTON_WIDTH_3_POS_2, BUTTON_HEIGHT_4_LINE_4, BUTTON_WIDTH_3, BUTTON_HEIGHT_4, COLOR_RED, F("Test"),
    TEXT_SIZE_22, FLAG_BUTTON_DO_BEEP_ON_TOUCH, 3, &GUISwitchPages);

#ifdef USE_TB6612_BREAKOUT_BOARD
    TouchButtonCameraOnOff.init(0, BUTTON_HEIGHT_4_LINE_4 - (TEXT_SIZE_22_HEIGHT + BUTTON_DEFAULT_SPACING_QUARTER), BUTTON_WIDTH_3,
    TEXT_SIZE_22_HEIGHT, COLOR_BLACK, F("Camera"), TEXT_SIZE_22, FLAG_BUTTON_DO_BEEP_ON_TOUCH | FLAG_BUTTON_TYPE_TOGGLE_RED_GREEN,
            false, &doCameraSupplyOnOff);
#endif

#ifdef ENABLE_RTTTL
    TouchButtonMelody.init(BUTTON_WIDTH_3_POS_2, BUTTON_HEIGHT_4_LINE_4 - (TEXT_SIZE_22_HEIGHT + BUTTON_DEFAULT_SPACING_QUARTER),
    BUTTON_WIDTH_3, TEXT_SIZE_22_HEIGHT, COLOR_BLACK, F("Melody"), TEXT_SIZE_22,
            FLAG_BUTTON_DO_BEEP_ON_TOUCH | FLAG_BUTTON_TYPE_TOGGLE_RED_GREEN, false, &doPlayMelody);
#endif

    TouchButtonLaser.init(BUTTON_WIDTH_3_POS_3, BUTTON_HEIGHT_4_LINE_4 - (TEXT_SIZE_22_HEIGHT + BUTTON_DEFAULT_SPACING_QUARTER),
    BUTTON_WIDTH_3, TEXT_SIZE_22_HEIGHT, COLOR_BLACK, F("Laser"), TEXT_SIZE_22,
            FLAG_BUTTON_DO_BEEP_ON_TOUCH | FLAG_BUTTON_TYPE_TOGGLE_RED_GREEN, false, &doLaserOnOff);

    SliderLaserPositionHorizontal.init(BUTTON_WIDTH_6_POS_6, 10, BUTTON_WIDTH_6, LASER_SLIDER_SIZE, 90, 90, COLOR_YELLOW,
    SLIDER_DEFAULT_BAR_COLOR, FLAG_SLIDER_SHOW_VALUE, &doLaserServoPosition);
    SliderLaserPositionHorizontal.setBarThresholdColor(COLOR_BLUE);
    // scale slider values
    SliderLaserPositionHorizontal.setScaleFactor(180.0 / LASER_SLIDER_SIZE); // Values from 0 to 180 degrees
    SliderLaserPositionHorizontal.setValueUnitString("\xB0");

#ifdef USE_PAN_TILT_SERVO
    SliderLaserPositionVertical.init(BUTTON_WIDTH_6_POS_5, 10, BUTTON_WIDTH_6, LASER_SLIDER_SIZE, 90, 0, COLOR_YELLOW,
    SLIDER_DEFAULT_BAR_COLOR, FLAG_SLIDER_SHOW_VALUE, &doVerticalLaserServoPosition);
    SliderLaserPositionHorizontal.setBarThresholdColor(COLOR_BLUE);
    // scale slider values
    SliderLaserPositionHorizontal.setScaleFactor(180.0 / LASER_SLIDER_SIZE); // Values from 0 to 180 degrees
    SliderLaserPositionHorizontal.setValueUnitString("\xB0");

    LaserTiltServo.attach(LASER_SERVO_TILT_PIN);
    LaserTiltServo.write(TILT_SERVO_MIN_VALUE);
#endif

    // initialize and set Laser pan servo
    LaserPanServo.attach(LASER_SERVO_PAN_PIN);
    LaserPanServo.write(90);
}

/*
 * Manual control page
 */
void drawHomePage(void) {
    drawCommonGui();

    BlueDisplay1.drawText(BUTTON_WIDTH_10_POS_4 + TEXT_SIZE_22_WIDTH, TEXT_SIZE_22_HEIGHT + TEXT_SIZE_22_HEIGHT, F("Control"),
    TEXT_SIZE_22, COLOR_BLUE, COLOR_NO_BACKGROUND);

    TouchButtonRobotCarStartStop.drawButton();
    TouchButtonLaser.drawButton();
    TouchButtonMelody.drawButton();
    TouchButtonCameraOnOff.drawButton();

    TouchButtonTestPage.drawButton();
    TouchButtonNextPage.drawButton();

    TouchButtonDirection.drawButton();
    TouchButtonCalibrate.drawButton();

    SliderLaserPositionHorizontal.drawSlider();
#ifdef USE_PAN_TILT_SERVO
    SliderLaserPositionVertical.drawSlider();
#endif
    SliderSpeed.drawSlider();
    SliderSpeedRight.drawSlider();
    SliderSpeedLeft.drawSlider();

    printMotorValues();

}

void startHomePage(void) {
    TouchButtonDirection.setPosition(BUTTON_WIDTH_8_POS_4, BUTTON_HEIGHT_8_LINE_4);
    TouchButtonCalibrate.setPosition(BUTTON_WIDTH_8_POS_5, BUTTON_HEIGHT_8_LINE_4);
    TouchButtonNextPage.setCaption(F("Automatic\nControl"));

    drawHomePage();
}

void loopHomePage(void) {
    displayVelocitySliderValues();

    if (EncoderMotor::ValuesHaveChanged) {
        EncoderMotor::ValuesHaveChanged = false;
        printMotorValues();
    }
}

void stopHomePage(void) {
    TouchButtonDirection.setPosition(BUTTON_WIDTH_8_POS_6, BUTTON_HEIGHT_8_LINE_5);
    TouchButtonCalibrate.setPosition(BUTTON_WIDTH_8_POS_6, BUTTON_HEIGHT_8_LINE_2);
    startStopRobotCar(false);
}

