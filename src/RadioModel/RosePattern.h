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

#ifndef __INET_ROSEPATTERN_H
#define __INET_ROSEPATTERN_H

#include "IAntennaPattern.h"

namespace RadioModel {

class INET_API RosePattern : public IAntennaPattern
{
  protected:
	// rose curve parameters
    double a;
	double n;
	double scaled_a;

	// radio parameters
	double mainLobeGain;
	double sideLobeGain;

  public:
	// methods to be re-implemented
	virtual void initialize();
    virtual double calculateGain(Coord& posA, Coord& posB);

};

}
#endif


