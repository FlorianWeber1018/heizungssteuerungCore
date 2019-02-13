/*
 * COMprotocol.h
 *
 * Created: 09.10.2017 21:16:22
 *  Author: Flo
 */


#ifndef PROTKOLDEF_H_
#define PROTKOLDEF_H_

	#define debuglevel 0


	#define number0 1	// internaly offset of +1
												// (to provide using nullterminated strings)

	#define number15 (number0+15)
	#define plus (number15+1)
	#define minus (plus+1)
	#define eot (minus+1)			//End of Transmission

	#define setVA0  (eot+1)
	#define setVA15 (setVA0+15)

	#define getVA0  (setVA15+1)
	#define getVA15 (getVA0+15)

	#define setCA0  (getVA15+1)
	#define setCA15 (setCA0+15)

	#define getCA0	(setCA15+1)
	#define getCA15	(getCA0+15)

	#define setVI0  (getCA15+1)
	#define setVI39 (setVI0+39)

	#define getVI0  (setVI39+1)
	#define getVI39 (getVI0+39)

	#define setCI0  (getVI39+1)
	#define setCI39	(setCI0+39)

	#define getCI0	(setCI39+1)
	#define getCI39	(getCI0+39)

	#define cmdfreeRam (getCI39+1)
    #define cmdDebug0 (cmdfreeRam+1)
    #define cmdDebug1 (cmdDebug0+1)
    #define cmdDebug2 (cmdDebug1+1)

    #define cmdresetMCU (cmdDebug2+1)

    #define SizeBufOutMax 512
    //#define SizeBufInMax 400

#endif /* PROTKOLDEF_H_ */
