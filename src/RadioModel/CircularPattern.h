//
// Paula Uribe
// INRIA - Sophia Antipolis
// September 2009
// A Circular shaped antenna pattern was implemented for the antenna gain, which is
// equivalent to an omni-directional antenna.
// After that the curve is normalized
// This pattern does not consider side/back lobes, since it is omni-directional.
//

#ifndef __INET_CIRCULARPATTERN_H
#define __INET_CIRCULARPATTERN_H

#include "IAntennaPattern.h"

namespace RadioModel {

class INET_API CircularPattern : public IAntennaPattern
{
  protected:
	// circle parameters
	double r;
	double scaled_r;

	// radio parameters
	double mainLobeGain;

  public:
	// methods to be re-implemented
	virtual void initialize();
    virtual double calculateGain(Coord& posA, Coord& posB);

};

}

#endif


