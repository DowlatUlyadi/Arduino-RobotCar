/*
 * AutonomousDrivePage.cpp
 *
 *  Contains all the GUI elements for autonomous driving.
 *
 *  Needs BlueDisplay library.
 *
 *  Created on: 13.05.2019
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

#include "RobotCarGui.h"
#include "RobotCar.h"

BDButton TouchButtonStepMode;
BDButton TouchButtonStep;
BDButton TouchButtonSingleScan;

BDButton TouchButtonTestUser;
BDButton TouchButtonBuiltInAutonomousDrive;

uint8_t sStepMode = MODE_CONTINUOUS;
bool sDoStep = false; // if true => do one step

bool sRunAutonomousDrive = false;
bool sUseBuiltInAutonomousDriveStrategy = true;

void setStepModeButtonCaption();
/*
 * Switches modes MODE_CONTINUOUS -> MODE_STEP_TO_NEXT_TURN -> MODE_SINGLE_STEP
 */
void setStepMode(uint8_t aStepMode) {
    if (aStepMode == MODE_SINGLE_STEP) {
        RobotCar.stopCar();
    } else if (aStepMode > MODE_SINGLE_STEP) {
        aStepMode = MODE_CONTINUOUS;
    }
    sStepMode = aStepMode;
    setStepModeButtonCaption();
    if (sActualPage == PAGE_AUTOMATIC_CONTROL) {
        TouchButtonStepMode.drawButton();
    }
}

void doNextStepMode(BDButton * aTheTouchedButton, int16_t aValue) {
    sStepMode++;
    setStepMode(sStepMode);
}

/*
 * enables next step
 */
void doStep(BDButton * aTheTouchedButton, int16_t aValue) {
    if (sStepMode == MODE_CONTINUOUS) {
        // switch to step mode MODE_SINGLE_STEP
        setStepMode( MODE_SINGLE_STEP);
    }
    sDoStep = true;
    /*
     * Start if not yet done
     */
    if (!sStarted) {
        startStopAutomomousDrive(true, sUseBuiltInAutonomousDriveStrategy);
    }
}

void doSingleScan(BDButton * aTheTouchedButton, int16_t aValue) {
    bool tInfoWasProcessed;
    clearPrintedForwardDistancesInfos();
    tInfoWasProcessed = fillForwardDistancesInfo(true, true);
    doWallDetection(true);
    sNextDegreesToTurn = doBuiltInCollisionDetection();
    drawCollisionDecision(sNextDegreesToTurn, CENTIMETER_PER_RIDE, false);
    if (!tInfoWasProcessed) {
        drawForwardDistancesInfos();
    }
}

void startStopAutomomousDrive(bool aDoStart, bool aDoInternalAutonomousDrive) {
    sRunAutonomousDrive = aDoStart;
    sUseBuiltInAutonomousDriveStrategy = aDoInternalAutonomousDrive;
    /*
     *  manage buttons
     */
    TouchButtonBuiltInAutonomousDrive.setValue(aDoStart, (sActualPage == PAGE_AUTOMATIC_CONTROL));

    bool tInternalAutonomousDrive = aDoStart;
    bool tExternalAutonomousDrive = aDoStart;
    if (aDoStart) {
        // decide which button to disable if started
        if (aDoInternalAutonomousDrive) {
            tExternalAutonomousDrive = false;
        } else {
            /*
             * Own test starts in mode SINGLE_STEP
             */
            setStepMode(MODE_SINGLE_STEP);
            tInternalAutonomousDrive = false;
        }
        sDoStep = true;
        resetPathData();
    }
    TouchButtonBuiltInAutonomousDrive.setValue(tInternalAutonomousDrive, (sActualPage == PAGE_AUTOMATIC_CONTROL));
    TouchButtonTestUser.setValue(tExternalAutonomousDrive, (sActualPage == PAGE_AUTOMATIC_CONTROL));

    startStopRobotCar(aDoStart);
}

void doStartStopAutomomousDrive(BDButton * aTheTouchedButton, int16_t aValue) {
    startStopAutomomousDrive(aValue, true);
}

void doStartStopTestUser(BDButton * aTheTouchedButton, int16_t aValue) {
    startStopAutomomousDrive(aValue, false);
}

void setStepModeButtonCaption() {
    if (sStepMode == MODE_CONTINUOUS) {
        TouchButtonStepMode.setCaption(F("Continuous\n->\nStep to turn"));
    } else if (sStepMode == MODE_STEP_TO_NEXT_TURN) {
        TouchButtonStepMode.setCaption(F("Step\n->\nSingle step"));
    } else {
        TouchButtonStepMode.setCaption(F("Single step\n->\nContinuous"));
    }
}

void initAutonomousDrivePage(void) {
    TouchButtonStepMode.init(0, 0, BUTTON_WIDTH_3_5, BUTTON_HEIGHT_4, COLOR_BLUE, "", TEXT_SIZE_11, FLAG_BUTTON_DO_BEEP_ON_TOUCH, 0,
            &doNextStepMode);
    setStepModeButtonCaption();

    TouchButtonSingleScan.init(0, BUTTON_HEIGHT_4_LINE_2, BUTTON_WIDTH_3_5, BUTTON_HEIGHT_4, COLOR_BLUE, F("Scan"),
    TEXT_SIZE_22, FLAG_BUTTON_DO_BEEP_ON_TOUCH, 0, &doSingleScan);

    TouchButtonStep.init(0, BUTTON_HEIGHT_4_LINE_3, BUTTON_WIDTH_3_5, BUTTON_HEIGHT_4, COLOR_BLUE, F("Step"),
    TEXT_SIZE_22, FLAG_BUTTON_DO_BEEP_ON_TOUCH, 0, &doStep);

    TouchButtonBuiltInAutonomousDrive.init(0, BUTTON_HEIGHT_4_LINE_4, BUTTON_WIDTH_3, BUTTON_HEIGHT_4, COLOR_RED,
            F("Start\nBuiltin"),
            TEXT_SIZE_22, FLAG_BUTTON_DO_BEEP_ON_TOUCH | FLAG_BUTTON_TYPE_TOGGLE_RED_GREEN,
            sRunAutonomousDrive && sUseBuiltInAutonomousDriveStrategy, &doStartStopAutomomousDrive);
    TouchButtonBuiltInAutonomousDrive.setCaptionForValueTrue(F("Stop"));

    TouchButtonTestUser.init(BUTTON_WIDTH_3_POS_2, BUTTON_HEIGHT_4_LINE_4, BUTTON_WIDTH_3, BUTTON_HEIGHT_4, COLOR_RED,
            F("Start\nUser"), TEXT_SIZE_22, FLAG_BUTTON_DO_BEEP_ON_TOUCH | FLAG_BUTTON_TYPE_TOGGLE_RED_GREEN,
            sRunAutonomousDrive && !sUseBuiltInAutonomousDriveStrategy, &doStartStopTestUser);
    TouchButtonTestUser.setCaptionForValueTrue(F("Stop\nUser"));

}

void drawAutonomousDrivePage(void) {
    drawCommonGui();

    BlueDisplay1.drawText(BUTTON_WIDTH_10_POS_4, TEXT_SIZE_22_HEIGHT + TEXT_SIZE_22_HEIGHT, F("Auto drive"), TEXT_SIZE_22,
    COLOR_BLUE, COLOR_NO_BACKGROUND);

    TouchButtonBackSmall.drawButton();

    TouchButtonStepMode.drawButton();
    TouchButtonSingleScan.drawButton();
    TouchButtonStep.drawButton();

    TouchButtonBuiltInAutonomousDrive.drawButton();
    TouchButtonTestUser.drawButton();
    TouchButtonNextPage.drawButton();

    drawForwardDistancesInfos();
    drawCollisionDecision(sNextDegreesToTurn, CENTIMETER_PER_RIDE, false);
}

void startAutonomousDrivePage(void) {
    TouchButtonTestUser.setValue(sRunAutonomousDrive && !sUseBuiltInAutonomousDriveStrategy);
    TouchButtonBuiltInAutonomousDrive.setValue(sRunAutonomousDrive && sUseBuiltInAutonomousDriveStrategy);
    setStepModeButtonCaption();

    TouchButtonBackSmall.setPosition(BUTTON_WIDTH_4_POS_4, 0);
    TouchButtonNextPage.setCaption(F("Show\nPath"));

    drawAutonomousDrivePage();
}

void loopAutonomousDrivePage(void) {

}

void stopAutonomousDrivePage(void) {

}

/*
 * Clear drawing area
 */
void clearPrintedForwardDistancesInfos() {
    BlueDisplay1.fillRectRel(US_DISTANCE_MAP_ORIGIN_X - US_DISTANCE_MAP_WIDTH_HALF,
    US_DISTANCE_MAP_ORIGIN_Y - US_DISTANCE_MAP_HEIGHT, US_DISTANCE_MAP_WIDTH_HALF * 2, US_DISTANCE_MAP_HEIGHT + 2, COLOR_WHITE);
}

/*
 * Draw values of ActualDistancesArray as vectors
 */
void drawForwardDistancesInfos() {
    color16_t tColor;
    uint8_t tActualDegrees = 0;
    /*
     * Clear drawing area
     */
    clearPrintedForwardDistancesInfos();
    for (int i = 0; i < NUMBER_OF_DISTANCES; ++i) {
        /*
         * Determine color
         */
        uint8_t tDistance = sForwardDistancesInfo.RawDistancesArray[i];
        tColor = COLOR_ORANGE;
        if (tDistance >= US_TIMEOUT_CENTIMETER) {
            tDistance = US_TIMEOUT_CENTIMETER;
            tColor = COLOR_GREEN;
        }
        if (tDistance > sCountPerScan) {
            tColor = COLOR_GREEN;
        } else if (tDistance < sCentimeterPerScan) {
            tColor = COLOR_RED;
        }

        /*
         * Draw line
         */
        BlueDisplay1.drawVectorDegrees(US_DISTANCE_MAP_ORIGIN_X, US_DISTANCE_MAP_ORIGIN_Y, tDistance, tActualDegrees, tColor, 3);
        tActualDegrees += DEGREES_PER_STEP;
    }
    doWallDetection(true);
}

void drawCollisionDecision(int aDegreeToTurn, uint8_t aLengthOfVector, bool aDoClear) {
    if (sActualPage == PAGE_AUTOMATIC_CONTROL) {
        color16_t tColor = COLOR_YELLOW;
        int tDegreeToDisplay = aDegreeToTurn;
        if (tDegreeToDisplay == 180) {
            tColor = COLOR_PURPLE;
            tDegreeToDisplay = 0;
        }
        if (aDoClear) {
            tColor = COLOR_WHITE;
        }
        BlueDisplay1.drawVectorDegrees(US_DISTANCE_MAP_ORIGIN_X, US_DISTANCE_MAP_ORIGIN_Y, aLengthOfVector, tDegreeToDisplay + 90,
                tColor);
        if (!aDoClear) {
            sprintf_P(sStringBuffer, PSTR("wall%4d\xB0 rotation: %3d\xB0 wall%4d\xB0"), sForwardDistancesInfo.WallLeftAngleDegree,
                    aDegreeToTurn, sForwardDistancesInfo.WallRightAngleDegree);
            BlueDisplay1.drawText(US_DISTANCE_MAP_ORIGIN_X - US_DISTANCE_MAP_WIDTH_HALF, US_DISTANCE_MAP_ORIGIN_Y + TEXT_SIZE_11,
                    sStringBuffer, TEXT_SIZE_11, COLOR_BLACK, COLOR_WHITE);
        }
    }
}

