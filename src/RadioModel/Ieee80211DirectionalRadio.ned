package DirectionalRadio.RadioModel;

//
// Paula Uribe
// INRIA - Sophia ANtipolis
// Septembre 2009
// A new module is created for simulating a directional antenna (radio). Is mainly based in the omni-directional radio (Ieee80211Radio, but
// new parameters have been added and also new methods to calculate the tx/rx angle and the tx/rx power.
//

import DirectionalRadio.RadioModel.AbstractRadioInterface;

simple Ieee80211DirectionalRadio like AbstractRadioInterface
{
    parameters:
        @class(RadioModel::Ieee80211DirectionalRadio);
        int channelNumber = default(0); 							// channel identifier
        double carrierFrequency @unit("Hz") = default(2.4GHz);		// carrier frequency (in Hz)
        double transmitterPower @unit("mW") = default(20mW); 		// power used for transmission of messages (in mW)
        double bitrate @unit("bps"); 								// (in bits/s)
        double thermalNoise @unit("dBm") = default(-110dBm);		// base noise level (dBm)
        double shadowingDeviation @unit("dB") = default(0dB);       // used by the shadowing model calculation
        double snirThreshold @unit("dB") = default(4dB); 			// if signal-noise ratio is below this threshold, frame is considered noise (in dB)
        double sensitivity @unit("dBm"); 							// received signals with power below sensitivity are ignored
         
        double pathLossAlpha = default(2);                          // used by the path loss calculation: to be overwriten by the radioChannel  
        string attenuationModel = default("PathLossReceptionModel");// Values "pathlost" or "tworay"
        string berTableFile=default("");
        @display("i=block/wrxtx");
        
        // Paula Uribe: add new parameters for directional antennas
        double beamWidth @unit("deg") = default(40deg); 			// beamwidth angle for the main lobe (in degrees)
        double mainLobeGain @unit("dB") = default(15dB); 			// main lobe maximum gain (in dB)
		double sideLobeGain @unit("dBi") = default(-5dBi);			// side lobe gain (in dB)
		double mainLobeOrientation @unit("deg") = default(90deg); 	// main lobe orientation angle (in degrees)
		double dBThreshold @unit("dB") = default(3dB);				// dB threshold for calculating tha antenna beamwith
		string patternType = default("RadioModel::CircularPattern");			// Values any curve implemented as antenna pattern 
        
    gates:
        input uppergateIn;   	// from higher layer protocol (MAC)
        output uppergateOut; 	// to decider (decider connects to higher layer protocol, i.e. the MAC)
        input radioIn; 			// to receive frames (AirFrame) on the radio channel
}

