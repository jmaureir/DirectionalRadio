//
// Paula Uribe
// INRIA - Sophia Antipolis
// September 2009
// A Folium shaped antenna pattern was implemented for the directional antenna gain.
// The curve is scaled first according to the beamwidth value.
// After that the curve is normalized, and the gain is calculated as the product of the
// normalized curve with the maximum antenna gain (main lobe gain).
// The side/back lobes were modeled as circle. The gain in a given direction will be
// the greater value between the circle and the folium.
//

#include "FoliumPattern.h"

Register_Class(RadioModel::FoliumPattern);

void RadioModel::FoliumPattern::initialize()
{
	if (this->radioRef != NULL)
	{
		beamWidth = (this->radioRef)->getBeamWidth();
		mainLobeGain = (this->radioRef)->getMainLobeGain();
		sideLobeGain = (this->radioRef)->getSideLobeGain();
		dBThreshold = (this->radioRef)->getDBThreshold();
		mainLobeOrientation = (this->radioRef)->getMainLobeOrientation();
	}
	else
	{
		perror("Radio Ref not valid");
	}

	a = par<double>("a");
	b = par<double>("b");

	scaled_b = 1;
	scaled_a = scaled_b * a / b ;

	double folium = - scaled_b * cos(M_PI - beamWidth) + 4 * scaled_a * cos(M_PI - beamWidth) * pow(sin(M_PI - beamWidth),2);
	double xCoord = fabs(folium * cos(M_PI - beamWidth));
	k = fabs(((mainLobeGain - dBThreshold) / mainLobeGain ) * (1 / xCoord));

}

double RadioModel::FoliumPattern::calculateGain(Coord& posA, Coord& posB)
{
	double distanceX = posA.x - posB.x;
	double distanceY = posA.y - posB.y;
	double gain = 0;
	double thetaPrima = (this->radioRef)->calculateAngle(distanceX, distanceY);

	double theta = - ( M_PI - mainLobeOrientation + thetaPrima );
	//double theta = ( M_PI + mainLobeOrientation + thetaPrima ); // to be used if the user wants to indicate the mainLobeOrientation angle according to the
	// conventional x-y axis.
	double mLGain = pow(10.0, mainLobeGain / 10.0);
	double sLGain = pow(10.0, sideLobeGain / 10.0);
	double folium = - scaled_b * cos (theta) + 4 * scaled_a * cos(theta) * pow(sin(theta), 2);
	bool sign = folium < 0 ;
	double xCoord = fabs(folium * cos(theta));
	double yCoord = k * fabs(folium * sin(theta));
	folium = sqrt( pow(xCoord,2) + pow(yCoord,2) );
	if (sign == true)
		folium = - folium;

	double r1 = mLGain * folium;
	double r2 = sLGain;

	if (r1 >= r2)
	{
		gain = r1;
		//std::cout << "USING MAIN LOBE GAIN = " << gain << endl;
	}
	else
	{
		gain = r2;
		//std::cout << "USING SIDE LOBE GAIN = " << gain << endl;
	}

	return gain;
}


