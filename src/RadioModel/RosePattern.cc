//
// Paula Uribe
// INRIA - Sophia Antipolis
// September 2009
// A Rose shaped antenna pattern was implemented for the directional antenna gain.
// The curve is normalized, and the gain is calculated as the product of the
// normalized curve with the maximum antenna gain (main lobe gain).
// The side/back lobes were modeled as circle. The gain in a given direction will be
// the greater value between the circle and the rose.
//

#include "RosePattern.h"

Register_Class(RadioModel::RosePattern);

void RadioModel::RosePattern::initialize()
{
	if (this->radioRef != NULL)
	{
		mainLobeGain = (this->radioRef)->getMainLobeGain();
		sideLobeGain = (this->radioRef)->getSideLobeGain();
	}
	else
	{
		perror("Radio Ref not valid");
	}

	a = par<double>("a");
	n = par<double>("n");

	scaled_a = 1 ;

}

double RadioModel::RosePattern::calculateGain(Coord& posA, Coord& posB)
{
	double distanceX = posA.x - posB.x;
	double distanceY = posA.y - posB.y;
	double gain = 0;
	double theta = (this->radioRef)->calculateAngle(distanceX, distanceY);

	double mLGain = pow(10.0, mainLobeGain / 10.0);
	double sLGain = pow(10.0, sideLobeGain / 10.0);
	double rose = fabs(scaled_a * sin(n * theta));
	//bool sign = rose < 0 ;
	//if (sign == true)
	//	cardioid = - cardioid;

	double r1 = mLGain * rose;
	double r2 = sLGain;

	if (r1 >= r2)
	{
		gain = r1;
		std::cout << "USING MAIN LOBE GAIN = " << gain << endl;
	}
	else
	{
		gain = r2;
		std::cout << "USING SIDE LOBE GAIN = " << gain << endl;
	}

	return gain;
}


