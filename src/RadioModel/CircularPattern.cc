//
// Paula Uribe
// INRIA - Sophia Antipolis
// September 2009
// A Circular shaped antenna pattern was implemented for the antenna gain, which is
// equivalent to an omni-directional antenna.
// After that the curve is normalized
// This pattern does not consider side/back lobes, since it is omni-directional.
//

#include "CircularPattern.h"

Register_Class(RadioModel::CircularPattern);

void RadioModel::CircularPattern::initialize()
{
	if (this->radioRef != NULL)
	{
		mainLobeGain = (this->radioRef)->getMainLobeGain();
	}
	else
	{
		perror("Radio Ref not valid");
	}

	r = par<double>("r");
	scaled_r = 1;

}

double RadioModel::CircularPattern::calculateGain(Coord& posA, Coord& posB)
{
	double mLGain = pow(10.0, mainLobeGain / 10.0);
	double gain = mLGain * scaled_r;

	return gain;
}
