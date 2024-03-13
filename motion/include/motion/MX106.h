/*
 *   MX106.h
 *
 *   Author: ROBOTIS
 *
 */

#ifndef _MX_106_H_
#define _MX_106_H_

//#define MX106_1024

namespace Robot
{
    class MX106
    {
    public:
        static const int MIN_VALUE;
#ifdef MX106_1024
        static const int CENTER_VALUE;
        static const int MAX_VALUE;
        static const double MIN_ANGLE;
        static const double MAX_ANGLE;
        static const double RATIO_VALUE2ANGLE;
        static const double RATIO_ANGLE2VALUE;

        static const int PARAM_BYTES;
#else
        static const int CENTER_VALUE;
        static const int MAX_VALUE;
        static const double MIN_ANGLE;
        static const double MAX_ANGLE;
        static const double RATIO_VALUE2ANGLE;
        static const double RATIO_ANGLE2VALUE;

        static const int PARAM_BYTES;
#endif

        static int GetMirrorValue(int value)       { return MAX_VALUE + 1 - value; }
        static double GetMirrorAngle(double angle) { return -angle; }

        static int Angle2Value(double angle) { return (int)(angle*RATIO_ANGLE2VALUE)+CENTER_VALUE; }
        static double Value2Angle(int value) { return (double)(value-CENTER_VALUE)*RATIO_VALUE2ANGLE; }

        // Address
#ifdef MX106_1024
        enum
        {
            // Add MX-106 specific addresses for 1024 resolution
            // ...
            MAXNUM_ADDRESS
        };
#else
        enum
        {
            // Add MX-106 specific addresses for 4096 resolution
            // ...
            MAXNUM_ADDRESS
        };
#endif
    };
}

#endif
