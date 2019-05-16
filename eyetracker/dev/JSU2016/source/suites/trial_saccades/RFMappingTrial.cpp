/**
 * @file RFMappingTrial.cpp
 * @brief 
 */

/*****************************************************************************
 * Includes
 *****************************************************************************/

#include <afxwin.h>		/* AfxMessageBox */
#include <ostream>
#include <algorithm>
#include <deque>
#include <memory>
#include <math.h>
#include "app/AppCore.h"
#include "utils/Math.h"
#include "utils/Grid.h"
#include "utils/Utils.h"
#include "SaccadeTrial.h"
#include "TrialSaccades.h"

/*****************************************************************************
 * Classes
 *****************************************************************************/

/**
 * @brief Interface to a calculator for fixating and deviated eye positions.
 * 
 * Calculations differ when computing locations for the center cross and
 * peripheral target.  In all cases, cache the location of the center cross by 
 * calling NewPoint(xx, yy).  
 *
 * For the center cross, call the virtual Get[Left/Right]Eye[Fix/Dev]
 * functions to calculate strabismic locations for the left and right eyes.  For 
 * example, if the cross location is nominally (0 deg, 0 deg) for cyclopean 
 * behavior, call
 * <code>
 *     eyePositionsInst.NewPoint(0,0);
 * </code>
 * Then, for a red cross (right eye), call
 * <code>
 *     eyePositionsInst.GetRightEyeFix(xDeg, yDeg);
 * </code>
 * The class uses the stored (approximate) deviation to calculate cross 
 * positions.
 *
 * To get the location of the deviated eye for use with a peripheral target
 * vector, functionality differs between approximation mode and eye-tracking 
 * mode.  In approximation mode, the same virtual Get[Left/Right]Eye[Fix/Dev]
 * functions are used.  In eye-tracking mode, however, the tracked position of
 * the eye is needed.  Thus the non-virtual function should be used: it will
 * return the tracked location for eye-tracking mode but dispatch to the
 * virtual functions for approximation mode.
 */
class CEyePositions
{
public:
	explicit CEyePositions(double devDeg);
	virtual ~CEyePositions();

	void NewPoint(double xDeg, double yDeg);
	double GetDeviation() const;

	virtual void GetLeftEyeFix(double& xDeg, double& yDeg)  const = 0;
	virtual void GetRightEyeFix(double& xDeg, double& yDeg) const = 0;
	virtual void GetLeftEyeDev(double& xDeg, double& yDeg)  const = 0;
	virtual void GetRightEyeDev(double& xDeg, double& yDeg) const = 0;

	/**
	 * @TODO Handle real-time eye tracking better.
	 * Store the CEyePositions object in the global CRFMapping object.
	 * Non-virtual methods in CEyePositions will return real-time eye position
	 * if RTET is enabled, or will dispatch to virtual approximation methods
	 * if RTET is not enabled.
	 */

protected:
	double m_devDeg;
	double m_xDeg;
	double m_yDeg;
};

/**
 * @brief Typedef for auto pointer to CEyePositions.
 */
typedef std::auto_ptr<CEyePositions> pEyePositions_t;

/****************************************************************************/

/**
 * @brief Calculator for "cyclopean" eye positions, where each actual eye 
 * position falls to the left or right of the cached point.
 *
 * For cyclopean eye fixation, the _Fix and _Dev functions return the same
 * values for respective eyes:
 * <code>
 *     double xx, yy;
 *
 *     GetLeftEyeFix(xx, yy);  // Equivalent to
 *     GetLeftEyeDev(xx, yy);  // each other.
 *
 *     GetRightEyeFix(xx, yy); // Equivalent to
 *     GetRightEyeDev(xx, yy); // each other.
 * </code>
 */
class CCyclopeanEyePositions : public CEyePositions
{
public:
	explicit CCyclopeanEyePositions(double devDeg);
	virtual ~CCyclopeanEyePositions();

	virtual void GetLeftEyeFix(double& xDeg, double& yDeg) const;
	virtual void GetRightEyeFix(double& xDeg, double& yDeg) const;
	virtual void GetLeftEyeDev(double& xDeg, double& yDeg) const;
	virtual void GetRightEyeDev(double& xDeg, double& yDeg) const;
};

/****************************************************************************/

/**
 * @brief Calculator for random eye positions, where one eye is fixated and
 * the other is deviated.
 */
class CRandomEyePositions : public CEyePositions
{
public:
	explicit CRandomEyePositions(double devDeg);
	virtual ~CRandomEyePositions();

	virtual void GetLeftEyeFix(double& xDeg, double& yDeg) const;
	virtual void GetRightEyeFix(double& xDeg, double& yDeg) const;
	virtual void GetLeftEyeDev(double& xDeg, double& yDeg) const;
	virtual void GetRightEyeDev(double& xDeg, double& yDeg) const;
};



/****************************************************************************/

/**
 * @brief Object to generate Saccade Trials with reciprocal coverage for each 
 * eye.
 */
class CBNRFMappingTrialGenerator : public CRFMappingTrialGenerator
{
public:
	CBNRFMappingTrialGenerator(const CRFMapping& rfMap,
		etu::pGrid_t& fixationGrid, etu::pGrid_t& peripheralGrid, 
		pEyePositions_t& eyePositions);
	virtual ~CBNRFMappingTrialGenerator();

	virtual void GenerateTrials(TrialPtrContainer_t& trials);

private:
	const CRFMap& m_rfMap;
	etu::pGrid_t m_fixationGrid;
	etu::pGrid_t m_peripheralGrid;
	pEyePositions_t m_eyePositions;
};

/*****************************************************************************
 * Member Functions
 *****************************************************************************/



/**
 * @brief Create new trial for given code, stim eye, and coordinates.
 *
 * @param stimColor Color for the stimulus.  See STIM_COLOR.
 * @param stimEye Code for which eye was stimulated.  See STIM_EYE.
 * @param ctrXDeg The X coordinate, in degrees, of center cross.
 * @param ctrYDeg The Y coordinate, in degrees, of center cross.
 * @param perXDeg The X coordinate, in degrees, of saccade target.
 * @param perYDeg The Y coordinate, in degrees, of saccade target.
 * @param perADeg The radial angle, in degrees, of saccade target.
 * @param perRDeg The radius (eccentricity), in degrees, of saccade target.
 */
CSaccadeTrial::CSaccadeTrial(STIM_COLOR stimColor, STIM_EYE stimEye,
							 double ctrXDeg, double ctrYDeg, double perXDeg, 
							 double perYDeg, double perADeg, double perRDeg)
: m_stimColor(stimColor), m_stimEye(stimEye), m_ctrXDeg(ctrXDeg), 
  m_ctrYDeg(ctrYDeg), m_perXDeg(perXDeg), m_perYDeg(perYDeg), 
  m_perADeg(perADeg), m_perRDeg(perRDeg)
{}

/**
 * @brief Clean up.
 */
CSaccadeTrial::~CSaccadeTrial() { /* Empty. */}

/****************************************************************************/

/**
 * @brief Store the deviation angle
 * @param devDeg Deviation angle between eyes, in degrees.
 */
CEyePositions::CEyePositions(double devDeg) 
: m_devDeg(devDeg), m_xDeg(0.0), m_yDeg(0.0) 
{ /* Empty */ }

/**
 * @brief Clean up.
 */
CEyePositions::~CEyePositions() { /* Empty */ }

/**
 * @brief Cache the new point.
 * @param xDeg X coordinate, in degrees.
 * @param yDeg Y coordinate, in degrees.
 */
void CEyePositions::NewPoint(double xDeg, double yDeg)
{
	m_xDeg = xDeg;
	m_yDeg = yDeg;
}

/**
 * @brief Read the stored deviation in degrees.
 */
double CEyePositions::GetDeviation() const
{
	return m_devDeg;
}


/****************************************************************************/

/**
 * @brief Clean up.
 */
CRFMappingTrialGenerator::~CRFMappingTrialGenerator() { /* Empty */ }

/**
 * @brief Make generator for given grids based on state of Trial Saccades 
 * model.
 * @param trialSaccades Trial Saccades model for state information.
 * @param fixationGrid Grid from which to take fixation points; takes ownership.
 * @param peripheralGrid Grid from which to take peripheral saccade target 
 * points; takes ownership.
 */
CRFMappingTrialGenerator* CRFMappingTrialGenerator::GetBNGenerator(
	const CRFMapping& rfMap, etu::pGrid_t& fixationGrid, 
	etu::pGrid_t& peripheralGrid, int swapInterval)
{
	return (new CBNRFMappingTrialGenerator(rfMap, fixationGrid, peripheralGrid, swapInterval));

}


/****************************************************************************/

/**
 * @brief Construct object to generate points for reciprocal eye coverage.
 * @param trialSaccades Reference to the trialSaccades model.
 * @param fixationGrid Grid for center fixation points; takes ownership.
 * @param peripheralGrid Grid for peripheral saccade targets; takes ownership.
 * @param eyePositions Eye position calculated; takes ownership.
 */
CBNRFMappingTrialGenerator::CBNRFMappingTrialGenerator(
	const CTrialSaccades& trialSaccades, etu::pGrid_t& fixationGrid, 
	etu::pGrid_t& peripheralGrid, int swapInterval)
: m_trialSaccades(trialSaccades), m_fixationGrid(fixationGrid), 
  m_peripheralGrid(peripheralGrid), m_swapInterval(swapInterval)
{}

/**
 * @brief Cleanup
 */
CBNRFMappingTrialGenerator::~CBNRFMappingTrialGenerator() 
{}

/**
 * @brief Create new trial given eye position, fixation point, and peripheral 
 * point.
 * @param stimEye The eye upon which to base the retinotopic behavior.
 * @param stimCode The type of trial to make.
 * @param eyePosition CEyePositions object to calculate eye positions; 
 *        point must be cached..
 * @param perVector SPoint struct describing the peripheral target vector.
 * @return The trial object; caller owns new memory.
 */
static CSaccadeTrial* newTrial(CSaccadeTrial::STIM_EYE stimEye,
							   CSaccadeTrial::STIM_CODE stimCode, 
							   const CEyePositions* eyePosition, 
							   const etu::SPoint& perVector)
{
	bool randomRight = etu::RandCoinFlip();
	double xCrossDeg, yCrossDeg;
	double xTargDeg = perVector.xDeg;
	double yTargDeg = perVector.yDeg;

	if (CSaccadeTrial::isCenterRed(stimCode)) {
		eyePosition->GetRightEyeFix(xCrossDeg, yCrossDeg);
	}
	else if (CSaccadeTrial::isCenterBlue(stimCode)) {
		eyePosition->GetLeftEyeFix(xCrossDeg, yCrossDeg);
	}
	/* Purple centers split half-half randomly. */
	else if (randomRight) {
		eyePosition->GetRightEyeFix(xCrossDeg, yCrossDeg);
	}
	else {
		eyePosition->GetLeftEyeFix(xCrossDeg, yCrossDeg);
	}

	/**
	 * @TODO Handle real-time eye tracking better.
	 * Store the CEyePositions object in the global CTrialSaccades object.
	 * Non-virtual methods in CEyePositions will return real-time eye position
	 * if RTET is enabled, or will dispatch to virtual approximation methods
	 * if RTET is not enabled.
	 */

	if (g_pTrialSaccades->getUseApprox()) {
		double xSaccEyeDeg, ySaccEyeDeg;
		
		switch (stimEye) {
			case CSaccadeTrial::EYE_RIGHT: 
				if (CSaccadeTrial::isCenterRed(stimCode)) {
					xSaccEyeDeg = xCrossDeg;
					ySaccEyeDeg = yCrossDeg;
				}
				else if (CSaccadeTrial::isCenterBlue(stimCode)) {
					eyePosition->GetRightEyeDev(xSaccEyeDeg, ySaccEyeDeg);
				}
				else if (randomRight) {
					xSaccEyeDeg = xCrossDeg;
					ySaccEyeDeg = yCrossDeg;
				}
				else {
					eyePosition->GetRightEyeDev(xSaccEyeDeg, ySaccEyeDeg);
				}
				break;

			case CSaccadeTrial::EYE_LEFT:
				if (CSaccadeTrial::isCenterRed(stimCode)) {
					eyePosition->GetLeftEyeDev(xSaccEyeDeg, ySaccEyeDeg);
				}
				else if (CSaccadeTrial::isCenterBlue(stimCode)) {
					xSaccEyeDeg = xCrossDeg;
					ySaccEyeDeg = yCrossDeg;
				}
				else if (!randomRight) {
					eyePosition->GetLeftEyeDev(xSaccEyeDeg, ySaccEyeDeg);
				}
				else {
					xSaccEyeDeg = xCrossDeg;
					ySaccEyeDeg = yCrossDeg;
				}
				break;
		}

		xTargDeg += xSaccEyeDeg;
		yTargDeg += ySaccEyeDeg;
	}

	return (new CSaccadeTrial(stimCode, stimEye, xCrossDeg, yCrossDeg, 
		xTargDeg, yTargDeg, perVector.aDeg, perVector.rDeg));
}

static bool isOverMidline(const CSaccadeTrial* pTrial, double devDeg)
{
	double halfDev = 0.5 * devDeg;
	double targetXDeg;
	const CSaccadeTrial::STIM_EYE stimEye = pTrial->getStimEye();

	/* Saccade trial contains a vector for the peripheral target location. We 
	 * must determine the eventual location of the peripheral target by 
	 * interpreting that vector starting at the stimulated eye, which is what 
	 * will happen at run-time. */
	targetXDeg = pTrial->getPerXDeg() + pTrial->getCtrXDeg();

	if ((stimEye == CSaccadeTrial::EYE_RIGHT) && pTrial->isCenterBlue()) {
		targetXDeg += devDeg;
	}
	else if ((stimEye == CSaccadeTrial::EYE_LEFT) && pTrial->isCenterRed()) {
		targetXDeg -= devDeg;
	}

	switch (pTrial->getStimCode()) {
		case CSaccadeTrial::CODE_C_RED_P_RED:
			return (targetXDeg < (pTrial->getCtrXDeg() - halfDev));

		case CSaccadeTrial::CODE_C_RED_P_BLUE: 
			return (targetXDeg > (pTrial->getCtrXDeg() - halfDev));

		case CSaccadeTrial::CODE_C_BLUE_P_RED: 
			return (targetXDeg < (pTrial->getCtrXDeg() + halfDev));

		case CSaccadeTrial::CODE_C_BLUE_P_BLUE:
			return (targetXDeg > (pTrial->getCtrXDeg() + halfDev));
			
		default:
			CString msg;
			msg.Format("Bad stim code to isOverMidline(): %s", stimCodeToString(pTrial->getStimCode()));
			AfxMessageBox((LPCTSTR)msg);
			return FALSE;
	}
}



static bool isCrossover(const CSaccadeTrial* pTrial){
	const CSaccadeTrial::STIM_EYE stimEye = pTrial->getStimEye();
	if((stimEye ==CSaccadeTrial::EYE_RIGHT)&& pTrial->isPeriphBlue()){
		return true;
	}else if((stimEye ==CSaccadeTrial::EYE_LEFT)&&pTrial->isPeriphRed()){
		return true;
	}else{
		return false;
	}

}



/**
 * @brief Generate a radial grid.
 * 
 * @param stimCode Code to use for the trial.
 * @param trials Container to receive the objects.
 * @param numGrids The number of grids to generate; default of 1.
 */
void CRetinotopicTrialGenerator::GenerateTrials(TrialPtrContainer_t& trials)
{
	const etu::GridIter_t fixEnd = m_fixationGrid->end();
	const etu::GridIter_t perEnd = m_peripheralGrid->end();
	/* Difficulty level calculations */
	const int numRepeatsStandard = CTrialSaccades::NUM_DIFFICULTY_LEVELS - 1;
	const int numRepeatsCrossover = m_trialSaccades.getDifficultyLevel();
	bool overMidline = FALSE;
	CSaccadeTrial* pSaccadeTrial;
	
	etu::GridIter_t fixIter = m_fixationGrid->begin();
	etu::GridIter_t perIter = m_peripheralGrid->begin();

	for ( ; fixIter != fixEnd; ++fixIter, perIter = m_peripheralGrid->begin()) 
	{
		m_eyePositions->NewPoint(fixIter->xDeg, fixIter->yDeg);

		for ( ; perIter != perEnd; ++perIter) 
		{	
			for (int eye = CSaccadeTrial::EYE_RIGHT; eye <= CSaccadeTrial::EYE_LEFT; ++eye)
			{
				if(!(eye==CSaccadeTrial::EYE_LEFT && g_pTrialSaccades->getStimRightEye()))
				{
					for (int code = CSaccadeTrial::CODE_C_RED_P_RED; code <= CSaccadeTrial::CODE_C_BLUE_P_BLUE; ++code)
					{
						int numRepeats, ix;

						pSaccadeTrial = newTrial((CSaccadeTrial::STIM_EYE)eye,
							(CSaccadeTrial::STIM_CODE)code, m_eyePositions.get(), *perIter);

						numRepeats = (isCrossover(pSaccadeTrial) ? numRepeatsCrossover : numRepeatsStandard);

						/* The loop will 
						between 0 and numRepeatsStandard 
						 * new heap objects and store pointers to each. Delete the
						 * template object afterward to avoid a memory leak. */
						for (ix = 0; ix < numRepeats; ++ix) {
							trials.push_back(new CSaccadeTrial(*pSaccadeTrial));
						}
						delete pSaccadeTrial;
					}
				}
			}
		}
	}

	std::random_shuffle(trials.begin(), trials.end());
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Put a trial to an out stream in a readable format.
 *
 * The trial's central and peripheral colors are written, followed by the 
 * dimensions.  The codes are finally printed as a logic check.
 *
 * @param out Output stream.
 * @param trial The trial.
 */
std::ostream& operator<< (std::ostream& out, const CSaccadeTrial& trial)
{
	out << (trial.isCenterRed() ? "Red" : (trial.isCenterBlue() ? "Blue" : "Purple")) << "\t"
		<< "Ctr Deg: (" << trial.getCtrXDeg() << ", " << trial.getCtrYDeg() << ") \t"
		<< (trial.isPeriphRed() ? "Red" : (trial.isPeriphBlue() ? "Blue" : "Purple")) << "\t"
		<< "Per Deg: (" << trial.getPerXDeg() << ", " << trial.getPerYDeg() << ") \t"
		<< trial.getStimCode() << std::endl;
	return out;
}

/**
 * @brief Convert a radial angle to its hex code.
 * @param angleDeg Radial angle with offset applied.
 * @param offsetDeg The offset.
 * @return The hex code; guaranteed to fill only the lowest byte.
 */
int angleToCode(double angleDeg, double offsetDeg)
{
	/* Remove the offset to normalize to the radial grid GUI. 
	 * Then take (angle % 360) just to be sure. */
	int angle = (int)(angleDeg - offsetDeg);
	int offset = (angle % 360) / 5;
	return ((0x21 + offset) & 0xFF);
}

/**
 * @brief Convert a radius to its hex code.
 * @param angleDeg Radius with offset applied.
 * @param offsetDeg The offset.
 * @return The hex code; guaranteed to fill only the lowest byte.
 */
int radiusToCode(double radiusDeg, double offsetDeg)
{
	int offset = (int)(radiusDeg - offsetDeg) / 5;
	return ((0x21 + offset) & 0xFF);
}

/**
 * @brief Get string description of trial type by stim code.
 * 
 * @param stimCode Stimulus code
 * @return string, for example ctrR perB, or ctrP perP.
 */
const char* stimCodeToString(CSaccadeTrial::STIM_CODE stimCode)
{
	static char* strings[] = {
		"Red  Red", 
		"Red  Blue", 
		"Blue Red", 
		"Blue Blue", 
		"Red  Prpl", 
		"Blue Prpl", 
		"Prpl Red", 
		"Prpl Blue", 
		"Prpl Prpl", 
		"unknown"};

	const char* pString;

	switch (stimCode) {
		case CSaccadeTrial::CODE_C_RED_P_RED:       pString = strings[0]; break;
		case CSaccadeTrial::CODE_C_RED_P_BLUE:      pString = strings[1]; break;
		case CSaccadeTrial::CODE_C_BLUE_P_RED:      pString = strings[2]; break;
		case CSaccadeTrial::CODE_C_BLUE_P_BLUE:     pString = strings[3]; break;
		case CSaccadeTrial::CODE_C_RED_P_PURPLE:    pString = strings[4]; break;
		case CSaccadeTrial::CODE_C_BLUE_P_PURPLE:   pString = strings[5]; break;
		case CSaccadeTrial::CODE_C_PURPLE_P_RED:    pString = strings[6]; break;
		case CSaccadeTrial::CODE_C_PURPLE_P_BLUE:   pString = strings[7]; break;
		case CSaccadeTrial::CODE_C_PURPLE_P_PURPLE: pString = strings[8]; break;
		default:                                    pString = strings[9]; break;
	}

	return pString;
}
