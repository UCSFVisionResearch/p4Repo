/**
 * @file RFMappingTrial.h
 * @brief 
 */

#ifndef RFMAPPINGTRIAL_H
#define RFMAPPINGTRIAL_H

/*****************************************************************************
 * Includes
 *****************************************************************************/

#include <ostream>
#include <deque>
#include <utility>
#include "utils/Grid.h"

/*****************************************************************************
 * Forward declaration
 *****************************************************************************/

class RFMapping;

/*****************************************************************************
 * Classes
 *****************************************************************************/

/**
 * @brief Encapsulates info about a Binoculoar RF Mapping trial.
 *
 * Member functions make queries easy.
 */
class CRFMappingTrial
{
public:
	/**
	 * @brief Constants for type of stimulus color.
	 * Values are a messaging-based convention for JRE's post-processing tools
	 * that run in Spike2.
	 */
	enum STIM_COLOR {
		CODE_C_RED       = 0, /**< Stim Color Red   */
		CODE_C_BLUE      = 1, /**< Stim Color Blue.   */
		CODE_C_PURPLE    = 2, /**< Stim Color Purple    */
		CODE_C_RAND_RED_BLUE     = 3, /**< Stim Color Randomly Red or Blue   */
		NUM_CODES                   /**< Number of codes; ALWAYS LAST */
	};

	/**
	 * @brief Constants for which eye is being rf mapped.
	 * Despite dichroic filters, we still may try to stimulate the left eye 
	 * with red or the right eye with blue.  Values are a messaging-based 
	 * convention for JRE's post-processing tools that run in Spike2. 
	 */
	enum STIM_EYE {
		EYE_RIGHT = 0,  /**< Right eye was stimulated. */
		EYE_LEFT  = 1,  /**< Left eye was stimulated. */
	};



	CRFMappingTrial(STIM_CODE stimCode, STIM_EYE stimEye, double ctrXDeg, 
		double ctrYDeg, double perXDeg, double perYDeg, double perADeg, 
		double perRDeg);
	/* Support implicit copy constructor. 
	CSaccadeTrial(const CSaccadeTrial&);    */
	~CSaccadeTrial();


	inline STIM_CODE getStimCode() const;
	inline STIM_EYE getStimEye() const;
	inline double getCtrXDeg() const;
	inline double getCtrYDeg() const;
	inline double getPerXDeg() const;
	inline double getPerYDeg() const;
	inline double getPerADeg() const;
	inline double getPerRDeg() const;

private:
	STIM_CODE m_stimCode;
	STIM_EYE m_stimEye;
	double m_ctrXDeg; /**< Center X in degrees. */
	double m_ctrYDeg; /**< Center Y in degrees. */
	double m_perXDeg; /**< Peripheral X in degrees. */
	double m_perYDeg; /**< Peripheral Y in degrees. */
	double m_perADeg; /**< Peripheral compass angle in degrees. */
	double m_perRDeg; /**< Peripheral radius in degrees. */

	/* Not supported. */
	CRFMappingTrial& operator=(const CRFMappingTrial& other);
};

/****************************************************************************/

/** 
 * @brief Typedef for container of pointers to CRFMappingTrial heap objects.
 **/
typedef std::deque<CRFMappingTrial*> TrialPtrContainer_t;

/****************************************************************************/

/**
 * @brief Interface for an object to generate RFMapping Trials.
 *
 * Also provides a static factory method for making generators.
 */
class CRFMappingTrialGenerator
{
public:
	virtual ~CRFMappingGenerator();

	/**
	 * @brief Generate trials based on parameters supplied at creation.
	 *
	 * @param trials The trial container in which to store generated trials.
	 */
	virtual void GenerateTrials(TrialPtrContainer_t& trials) = 0;

	/**
	 * @brief Create a generator.
	 *
	 * @param trialSaccades Reference to the Trial Saccades model that holds 
	 * all state parameters.
	 * @param fixationGrid The grid object for fixation cross locations.
	 * @param peripheralGrid The grid object for saccade target locations.
	 * @return A heap object implementing this interface.
	 */
	static CRFMappingTrialGenerator* GetBNGenerator(
		const CRFMapping& rfMapping, etu::pGrid_t& fixationGrid, 
		etu::pGrid_t& peripheralGrid, int swapInterval);
};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

std::ostream& operator<< (std::ostream& out, const CRFMappingTrial& trial);
int angleToCode(double angleDeg, double offsetDeg);
int radiusToCode(double radiusDeg, double offsetDeg);
const char* stimCodeToString(CSaccadeTrial::STIM_CODE stimCode);

/*****************************************************************************
 * Inline Member Functions
 *****************************************************************************/


/** Get stim code for this trial. */
CRFMappingTrial::STIM_CODE CRFMappingTrial::getStimCode() const 
{ return m_stimCode; }

/** Get stim eye for this trial. */
CRFMappingTrial::STIM_EYE CRFMappingTrial::getStimEye() const
{ return m_stimEye; }

/** Get center X coordinate, in degrees, for this trial. */
double CRFMappingTrial::getCtrXDeg() const { return m_ctrXDeg; }

/** Get center Y coordinate, in degrees, for this trial. */
double CRFMappingTrial::getCtrYDeg() const { return m_ctrYDeg; }

/** Get peripheral X coordinate, in degrees, for this trial. */
double CRFMappingTrial::getPerXDeg() const { return m_perXDeg; }

/** Get peripheral Y coordinate, in degrees, for this trial. */
double CRFMappingTrial::getPerYDeg() const { return m_perYDeg; }

/** Get peripheral radial angle, in degrees, for this trial. */
double CRFMappingTrial::getPerADeg() const { return m_perADeg; }

/** Get peripheral radius, in degrees, for this trial. */
double CRFMappingTrial::getPerRDeg() const { return m_perRDeg; }


#endif // SACCADETRIAL_H
