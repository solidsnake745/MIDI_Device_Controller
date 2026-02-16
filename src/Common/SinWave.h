#ifndef SinWave_h
	#define SinWave_h
	
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	
	/// @brief Shared class for sine wave values
	/// @details Used to centralize logic and data.
	class SinWave
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_SINWAVE);
		
		//Constructor(s)
		SinWave(); //Disallow creating an instance		
		
		/// @brief Stores the sine values for the first quadrant
		/// @details Values are integer approximations by multiplying the original decimal values by 10000.
		/// Takes up less space than storing entire decimal values and is accurate enough for our purposes.
		/// Only the first quadrant is stored and then mirrored/inverted to calculate the rest.
		inline constexpr static uint32_t SIN_INT_APPROX[91] = 
		{
			//1		2		3		4		5		6		7		8		9		10
			0,		174,	348,	523,	697,	871,	1045,	1218,	1391,	1564,	//1
			1736,	1908,	2079,	2249,	2419,	2588,	2756,	2923,	3090,	3255,	//2
			3420,	3583,	3746,	3907,	4067,	4226,	4383,	4539,	4694,	4848,	//3
			5000,	5150,	5299,	5446,	5591,	5735,	5877,	6018,	6156,	6293,	//4
			6427,	6560,	6691,	6819,	6946,	7071,	7193,	7313,	7431,	7547,	//5
			7660,	7771,	7880,	7986,	8090,	8191,	8290,	8386,	8480,	8571,	//6
			8660,	8746,	8829,	8910,	8987,	9063,	9135,	9205,	9271,	9335,	//7
			9396,	9455,	9510,	9563,	9612,	9659,	9702,	9743,	9781,	9816,	//8
			9848,	9876,	9902,	9925,	9945,	9961,	9975,	9986,	9993,	9998,	//9
			10000
		};
		
		inline static float getFirstQuadValue(uint16_t d) { return SIN_INT_APPROX[d] / 10000.0; };
		
		public:
		
			/// @brief Gets the sine value for the given degree
			/// @param d Degree to get the value for (0 to 360)
			///	@details Invalid values (>360) default to 360
			static float getSinValue(uint16_t d);

			/// @brief Prints out values of sine for degrees 0 to 360
			static void printSinValues();
	};
	
#endif
