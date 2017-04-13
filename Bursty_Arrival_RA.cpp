#include <iostream>
#include <fstream>
#include "lcgrand.c"
#include <vector>
#include <cmath>
#include <time.h>


//lcgrand period can achive 10^7
using namespace std;



typedef struct deviceM
{
	int deviceID;
	int preamble;		//record MS choosen preamble
	int nthPreamble;	// record preamble retransmission times
	int initArrivalTime;
	int arrivalTime;	// Update MS arrivalTime
	int delay;			// record Successful MS delay time
	bool preambleSuccess; // Check if preamble succes
	bool msgSuccess;	// Check if Msg succes
	bool exclude; // check if MS > NPTmax & MS finish RA procedure
} deviceM;

deviceM* Mdevice;

/*Analysis*/
void analArrivalMode1();
void analArrivalMode2();
void analColliProb();
int GetDeltaTE(int);

long long int power(int );
double ProbofBeta(int , int );

/*Simulation*/
void arrivalMode2();



bool trafficMode1 = false; // true:Mode1, false:Mode2
int g_T_RA_REP=5, g_SimTimes=10; // Interval between two successive random-access slots(sub-frame)
int g_t1=4, g_RAOs=54, g_TRAR=2, g_WRAR=5, g_NRAR=3, g_WBO=21;//subframe
int g_NPTmax=10, g_TCR=48, g_NHARQ=5, g_THARQ=4, g_TRQ=4, g_TA_RP=1, g_TRP=1;
double g_ProbRetransMsg=0.1;
double g_ProbErrMsg=0;

int g_TRAmax= 1+(g_NPTmax-1) * ceil((g_TRAR+g_WRAR+g_WBO)/g_T_RA_REP) * g_T_RA_REP;


vector<int> g_Tp(2); //Period of arrival process (sub-frame)
vector<int> M(3); //# of MS devices


int main(void)
{
	double simPc=0, simPs=0, simDa=0;
	g_Tp[0]=60000; //Traffic Mode1
	g_Tp[1]=10000;//Traffic Mode2
	M[0]=5000;
	M[1]=10000;
	M[2]=30000;
	int msg4Fail=0;
	for (int i = 0; i < g_NHARQ-1; ++i)
	{
		msg4Fail=msg4Fail+pow(g_ProbRetransMsg,i)*(1 - g_ProbRetransMsg)*pow(g_ProbRetransMsg,g_NHARQ);
	}
	g_ProbErrMsg=pow(g_ProbRetransMsg,g_NHARQ)+msg4Fail;
	// cout << g_ProbErrMsg << endl;	g_ProbErrMsg=1e-005
	// system("pause");
	vector<int> numPreambleArrival(g_Tp[0]/g_T_RA_REP);
	vector<int> RAR(20);
	int cntRAR=0;
	analArrivalMode1(); //Get Analysis Result
	// analArrivalMode2();
	// analColliProb();// Mode1&2
	//Traffic Mode1
	if(trafficMode1)
	{
		int sim1RaSlot=g_Tp[0]/g_T_RA_REP; //12000
		ofstream filePtrSimArrMode1;//check arrival model 1
		ofstream filePtrSimPcMode1, filePtrSimPsMode1, filePtrSimDaMode1;
		ofstream filePtrSimAccumNthPreamble, filePtrSimAccumAccessDelay;
		filePtrSimArrMode1.open("SimArrivalMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
		filePtrSimPcMode1.open("SimPcMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
		filePtrSimPsMode1.open("SimPsMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
		filePtrSimDaMode1.open("SimDaMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
		filePtrSimAccumNthPreamble.open("SimAccumNthPreambleMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
		filePtrSimAccumAccessDelay.open("SimAccumAccessDelayMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
		for(int index=0;index<3;++index)
		{
			double simPc=0, simPs=0, simDa=0;
			int raSlot=0, cntBackoff=0;
			int collidedRAOs=0, successMS=0;//For Pc and Ps statistic
			Mdevice = new deviceM[M[index]];
			// Initial Arrival Mode/Preamble Seleciton
			for (int j = 0; j < M[index]; ++j)
			{
				Mdevice[j].deviceID = j;
				Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2)); //0-53
				Mdevice[j].nthPreamble = 1;
				Mdevice[j].arrivalTime = (int)((sim1RaSlot-1)*lcgrand(5)); //0-11999(ra slot)
				Mdevice[j].initArrivalTime = Mdevice[j].arrivalTime;
				Mdevice[j].delay = 0;
				Mdevice[j].preambleSuccess=false;
				Mdevice[j].msgSuccess=false;
				Mdevice[j].exclude=false;
				//cout<<"preamble:"<<Mdevice[j].preamble<<"arrivalTime:"<<Mdevice[j].arrivalTime <<endl;
				//system("pause");
			}
			// for (int i = 0; i < sim1RaSlot; ++i)
			// {
			// 	for (int j = 0; j < M[index]; ++j)
			// 	{
			// 		if(Mdevice[j].initArrivalTime==i)	++numPreambleArrival[i];
			// 	}
			// }
			// for (int i = 0; i < sim1RaSlot; ++i)
			// {
			// 	filePtrSimArrMode1 << i <<","<<numPreambleArrival[i]/(double)10<<endl;
			// }
			int simIR=0, simTp=0, simTRAI=0, simTE=0, offsetTp=0;
			int maxIndex=0;
			for (int j = 1; j < M[index]; ++j)
			{
				if(Mdevice[maxIndex].initArrivalTime<Mdevice[j].initArrivalTime)	maxIndex=j;
			}
			simTp=Mdevice[maxIndex].initArrivalTime;
			if((simTp-g_t1)%5==0)	simTE=0;
			else	simTE=g_T_RA_REP-((simTp-g_t1)%5);
			simTRAI=simTp+simTE+g_TRAmax;
			offsetTp = g_t1+(simTp+simTE) * 5 + g_TRAmax;
			simIR=(offsetTp-g_t1)/g_T_RA_REP;
			// cout<<(offsetTp-g_t1)/g_T_RA_REP<<endl;
			// cout<<"Tp:"<<simTp<<endl;
			// cout<<"TE:"<<simTE<<endl;
			// cout<<"TRAmax:"<<g_TRAmax<<endl;
			// cout<<"TRAI:"<<simTRAI<<endl;
			// cout<<"offsetTp:"<<offsetTp<<endl;
			// system("pause");


			for (int i = 0; i < offsetTp; ++i)// time slot:60000...60225
			{
				if((i+1)%5==0)//4,9,14,19....59999...time slot => ith ra slot
				{
					for (int j = 0; j < M[index]; ++j)//M[index]=5000,10000,30000 devices
					{
						if(Mdevice[j].arrivalTime==raSlot&&!(Mdevice[j].exclude)&&!(Mdevice[j].msgSuccess))
						{
							// cout<<"raSlot: "<<raSlot<<endl;
							// system("pause");
							int n = Mdevice[j].nthPreamble;
							double probDetecPreamble = 1 - 1/exp(n);
							if(lcgrand(7) <= probDetecPreamble)//6,11..detect preamble slot
							{
								// Mdevice[j].preamble
								for (int k = 0; k < M[index]; ++k)
								{
									if (j!=k&&Mdevice[k].arrivalTime==raSlot&&!(Mdevice[k].exclude)&&!(Mdevice[j].msgSuccess))
									{
										if(Mdevice[j].preamble==Mdevice[k].preamble)//collision detection
										{
											int randBackoff=lcgrand(7) * g_WBO;
											Mdevice[k].arrivalTime = ceil((i+randBackoff+g_TRAR-g_t1)/5);
											if(Mdevice[k].arrivalTime<=raSlot)	++Mdevice[k].arrivalTime;

											++Mdevice[k].nthPreamble;
											if(Mdevice[k].nthPreamble>g_NPTmax)	Mdevice[k].exclude=true;

											Mdevice[k].preambleSuccess=false;
											Mdevice[k].preamble = (int)((g_RAOs-1) * lcgrand(2));

											++cntBackoff;
										}
									}
								}
								if(cntBackoff==0) // Mdevice[j] no collision
								{
									// cout<<"cntRAR:"<<cntRAR<<endl;
									// cout <<"ra slot:"<<raSlot<<endl;
									// cout << "deviceID:"<<j<<endl;
									// system("pause");
									RAR[cntRAR]=Mdevice[j].deviceID;
									// Mdevice[j].preambleSuccess=true;
									// RAR.push_back (Mdevice[j].deviceID);
									cntRAR++;
								}
								else
								{
									++collidedRAOs;
									int randBackoff=lcgrand(7) * g_WBO;
									Mdevice[j].arrivalTime = ceil((i+randBackoff-g_t1+g_TRAR)/5);
									if(Mdevice[j].arrivalTime<=raSlot)	++Mdevice[j].arrivalTime;

									++Mdevice[j].nthPreamble;
									if(Mdevice[j].nthPreamble>g_NPTmax)	Mdevice[j].exclude=true;

									Mdevice[j].preambleSuccess=false;
									Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2));

									cntBackoff=0;
								}
							}
							else//not detect by BS
							{
								int randBackoff=lcgrand(7) * g_WBO;
								Mdevice[j].arrivalTime = ceil((i+randBackoff-g_t1+g_TRAR)/5);
								if(Mdevice[j].arrivalTime<=raSlot)	++Mdevice[j].arrivalTime;

								++Mdevice[j].nthPreamble;
								if(Mdevice[j].nthPreamble>g_NPTmax)	Mdevice[j].exclude=true;

								Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2));

							}
						}
					}

					if(cntRAR>0)	RAR.resize(cntRAR);
					else if(cntRAR==0)	RAR.clear();

					while(RAR.size()>15)//RAR[position]: device ID
					{
						int position =(int)lcgrand(10) * (RAR.size()-1);
						int randBackoff=lcgrand(7) * g_WBO;
						Mdevice[RAR[position]].arrivalTime=ceil((i+randBackoff-g_t1+g_TRAR+g_WRAR)/5);
						++Mdevice[RAR[position]].nthPreamble;
						if(Mdevice[RAR[position]].nthPreamble>g_NPTmax)	Mdevice[RAR[position]].exclude=true;
						Mdevice[RAR[position]].preamble = (int)((g_RAOs-1) * lcgrand(2));
						RAR.erase (RAR.begin()+position);
					}
					for (int j = 0; j < RAR.size(); ++j)
					{
						Mdevice[RAR[j]].preambleSuccess=true;
						// cout<<"RAR Success"<<endl;
						// cout <<"ra slot:"<<raSlot<<endl;
						// cout << "device ID:" << RAR[j]<<endl;
						// system("pause");
					}
					RAR.clear ();
					RAR.resize (20);
					cntRAR=0;
					++raSlot;
				}
				if((i-2)%5==0&&i>10)//12,17,22,27...59997 Msg start transmission
				{
					for (int j = 0; j < M[index]; ++j)//M[0]=5000 devices
					{
						// if(Mdevice[j].preambleSuccess=true&&!(Mdevice[j].msgSuccess)&&Mdevice[j].initArrivalTime<i)
						if(Mdevice[j].preambleSuccess&&!(Mdevice[j].msgSuccess)&&!(Mdevice[j].exclude))
						{
							if(lcgrand(7)>g_ProbErrMsg)//Msg3&4 success
							{
								Mdevice[j].msgSuccess=true;
								Mdevice[j].delay=i+21-(g_t1+Mdevice[j].initArrivalTime * 5);// 9 or 22
								// cout << "Timeslot: "<<i << endl;
								// cout <<"deviceID:"<<j<<endl;
								// cout << "initArrivalTime:"<<Mdevice[j].initArrivalTime<<endl;
								// cout << "delay:"<<Mdevice[j].delay << endl;
								// system("pause");
								// Mdevice[j].exclude=true;
							}
							else
							{
								int randBackoff=lcgrand(7) * g_WBO;
								Mdevice[j].arrivalTime = ceil((i+48+randBackoff-g_t1)/5);
								if(Mdevice[j].arrivalTime<=raSlot)	Mdevice[j].arrivalTime=raSlot;

								++Mdevice[j].nthPreamble;
								if(Mdevice[j].nthPreamble>g_NPTmax)	Mdevice[j].exclude=true;

								Mdevice[j].msgSuccess=false;
								Mdevice[j].preambleSuccess=false;
								Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2));
							}
						}
					}
				}
			}
		for (int i = 0; i < M[index]; ++i)
		{
			if(Mdevice[i].msgSuccess)
			{
				++successMS;
				simDa = simDa + Mdevice[i].delay;
			}
		}
		int cntNthPramble=0;
		for (int i = 1; i <= g_NPTmax; ++i)
		{
			for (int j = 0; j < M[index]; ++j)
			{
				if(Mdevice[j].msgSuccess&&Mdevice[j].nthPreamble<=i)	++cntNthPramble;
			}
			filePtrSimAccumNthPreamble << i << "," << cntNthPramble/(double)successMS<<endl;
			cntNthPramble=0;
		}
		int cntAccessDelay=0;
		for (int i = 0; i <= 275; ++i)
		{
			for (int j = 0; j < M[index]; ++j)
			{
				if(Mdevice[j].msgSuccess&&Mdevice[j].delay<=i)	++cntAccessDelay;
			}
			filePtrSimAccumAccessDelay << i << "," << cntAccessDelay/(double)successMS<<endl;
			cntAccessDelay=0;
		}
		simPc = collidedRAOs/(double)(simIR * g_RAOs);
		simPs = successMS/(double)M[index];
		simDa = simDa/(double)successMS;
		// cout <<"collidedRAOs: "<<collidedRAOs<<"successMS:"<<successMS<<endl;
		// system("pause");
		filePtrSimPcMode1 << M[index] << "," << simPc << endl;
		filePtrSimPsMode1 << M[index] << "," << simPs << endl;
		filePtrSimDaMode1 << M[index] << "," << simDa << endl;
		delete[] Mdevice;
		}
		filePtrSimArrMode1.close();
		filePtrSimPcMode1.close();
		filePtrSimPsMode1.close();
		filePtrSimDaMode1.close();
		filePtrSimAccumNthPreamble.close();
		filePtrSimAccumAccessDelay.close();
	}//traffic Model 1
	if(!trafficMode1)//traffic Model 2
	{
		int sim1RaSlot=g_Tp[1]/g_T_RA_REP; //2000
		// ofstream filePtrSimArrMode2, filePtrSimArrMode2_CDF;//check arrival model 2
		ofstream filePtrSimPcMode2, filePtrSimPsMode2, filePtrSimDaMode2;
		ofstream filePtrSim2AccumNthPreamble, filePtrSim2AccumAccessDelay;
		// filePtrSimArrMode2_CDF.open("SimAccumArrivalMode2_M_RA_Slot_1_2000.csv", ios::out);
		// filePtrSimArrMode2.open("SimArrivalMode2_M_RA_Slot_1_2000.csv", ios::out);
		filePtrSimPcMode2.open("SimPcMode2_M_RA_Slot_1_2000.csv", ios::out);
		filePtrSimPsMode2.open("SimPsMode2_M_RA_Slot_1_2000.csv", ios::out);
		filePtrSimDaMode2.open("SimDaMode2_M_RA_Slot_1_2000.csv", ios::out);
		filePtrSim2AccumNthPreamble.open("SimAccumNthPreambleMode2_M_RA_Slot_1_2000.csv", ios::out);
		filePtrSim2AccumAccessDelay.open("SimAccumAccessDelayMode2_M_RA_Slot_1_2000.csv", ios::out);
		int simTime=1;
		for(int index=0;index<3;++index)
		{
			double simPc=0, simPs=0, simDa=0;
			vector<double> simFm(10), simGd(276);
			for (int z = 0; z < simTime; ++z)
			{
				// double simPc=0, simPs=0, simDa=0;
				double tempDa=0;
				int raSlot=0, cntBackoff=0;
				int collidedRAOs=0, successMS=0;//For Pc and Ps statistic
				Mdevice = new deviceM[M[index]];
				// vector<int> NumArrivalSlot(sim1RaSlot);

				// int sumArrival=0;
				// Initial Arrival Mode
			   	for(int i=0;i<sim1RaSlot;i++)
				{
					double betaPDFforithRAslot=ProbofBeta(i+1,sim1RaSlot);
					for(int j=0; j<M[index]; j++)
					{
						// for (int k = 0; k < simTime; ++k)
						// {
				            if(lcgrand(5)<=betaPDFforithRAslot)
				            {
								Mdevice[j].arrivalTime = i; //0-1999(ra slot)
								// NumArrivalSlot[i]++;
				            }
				        // }
			        }
			    }
			 //    int temp=0;
				// for (int i = 0; i < sim1RaSlot; ++i)
				// {
				// 	temp=(int)round(NumArrivalSlot[i]/(double)simTime);
				// 	filePtrSimArrMode2<<i<<","<<temp<<endl;
				// 	sumArrival=sumArrival+temp;
				// 	filePtrSimArrMode2_CDF<<i<<","<<sumArrival<<endl;
				// }
			 //    filePtrSimArrMode2.close();
			 //    filePtrSimArrMode2_CDF.close();
			 //    return 0;




			    //Initilize and Preamble Seleciton
				for (int j = 0; j < M[index]; ++j)
				{
					Mdevice[j].deviceID = j;
					Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2)); //0-53
					Mdevice[j].nthPreamble = 1;
					// Mdevice[j].arrivalTime = (int)((sim1RaSlot-1)*lcgrand(5)); //0-1999(ra slot)
					Mdevice[j].initArrivalTime = Mdevice[j].arrivalTime;
					Mdevice[j].delay = 0;
					Mdevice[j].preambleSuccess=false;
					Mdevice[j].msgSuccess=false;
					Mdevice[j].exclude=false;
					//cout<<"preamble:"<<Mdevice[j].preamble<<"arrivalTime:"<<Mdevice[j].arrivalTime <<endl;
					//system("pause");
				}
				// for (int i = 0; i < sim1RaSlot; ++i)
				// {
				// 	for (int j = 0; j < M[index]; ++j)
				// 	{
				// 		if(Mdevice[j].initArrivalTime==i)	++numPreambleArrival[i];
				// 	}
				// }
				// for (int i = 0; i < sim1RaSlot; ++i)
				// {
				// 	filePtrSimArrMode1 << i <<","<<numPreambleArrival[i]/(double)10<<endl;
				// }
				int simIR=0, simTp=0, simTRAI=0, simTE=0, offsetTp=0;
				int maxIndex=0;
				for (int j = 1; j < M[index]; ++j)
				{
					if(Mdevice[maxIndex].initArrivalTime<Mdevice[j].initArrivalTime)	maxIndex=j;
				}
				simTp=Mdevice[maxIndex].initArrivalTime;
				if((simTp-g_t1)%5==0)	simTE=0;
				else	simTE=g_T_RA_REP-((simTp-g_t1)%5);
				simTRAI=simTp+simTE+g_TRAmax;
				offsetTp = g_t1+(simTp+simTE) * 5 + g_TRAmax; // offsetTp=simTRAI
				simIR=(offsetTp-g_t1)/g_T_RA_REP; // total # of ra slot
				// cout<<(offsetTp-g_t1)/g_T_RA_REP<<endl;
				// cout<<"Tp:"<<simTp<<endl;
				// cout<<"TE:"<<simTE<<endl;
				// cout<<"TRAmax:"<<g_TRAmax<<endl;
				// cout<<"TRAI:"<<simTRAI<<endl;
				// cout<<"offsetTp:"<<offsetTp<<endl;
				// system("pause");

				// system("pause");
				for (int i = 0; i < offsetTp; ++i)// time slot:0...10XXX slot
				{
					if((i+1)%5==0)//4,9,14,19......time slot => ith ra slot
					{
						for (int j = 0; j < M[index]; ++j)//M[index]=5000,10000,30000 devices
						{
							if(Mdevice[j].arrivalTime==raSlot&&!(Mdevice[j].exclude)&&!(Mdevice[j].msgSuccess))
							{
								// cout<<"raSlot: "<<raSlot<<endl;
								// system("pause");
								int n = Mdevice[j].nthPreamble;
								double probDetecPreamble = 1 - 1/exp(n);
								if(lcgrand(7) <= probDetecPreamble)//6,11..detect preamble slot
								{
									// Mdevice[j].preamble
									for (int k = 0; k < M[index]; ++k)
									{
										if (j!=k&&Mdevice[k].arrivalTime==raSlot&&!(Mdevice[k].exclude)&&!(Mdevice[j].msgSuccess))
										{
											if(Mdevice[j].preamble==Mdevice[k].preamble)//collision detection
											{
												int randBackoff=lcgrand(7) * g_WBO;
												Mdevice[k].arrivalTime = ceil((i+randBackoff+g_TRAR-g_t1)/5);
												if(Mdevice[k].arrivalTime<=raSlot)	++Mdevice[k].arrivalTime;

												++Mdevice[k].nthPreamble;
												if(Mdevice[k].nthPreamble>g_NPTmax)	Mdevice[k].exclude=true;

												Mdevice[k].preambleSuccess=false;
												Mdevice[k].preamble = (int)((g_RAOs-1) * lcgrand(2));

												++cntBackoff;
											}
										}
									}
									if(cntBackoff==0) // Mdevice[j] no collision
									{
										// cout<<"cntRAR:"<<cntRAR<<endl;
										// cout <<"ra slot:"<<raSlot<<endl;
										// cout << "deviceID:"<<j<<endl;
										// system("pause");
										RAR[cntRAR]=Mdevice[j].deviceID;
										// Mdevice[j].preambleSuccess=true;
										// RAR.push_back (Mdevice[j].deviceID);
										cntRAR++;
									}
									else
									{
										++collidedRAOs;
										int randBackoff=lcgrand(7) * g_WBO;
										Mdevice[j].arrivalTime = ceil((i+randBackoff-g_t1+g_TRAR)/5);
										if(Mdevice[j].arrivalTime<=raSlot)	++Mdevice[j].arrivalTime;

										++Mdevice[j].nthPreamble;
										if(Mdevice[j].nthPreamble>g_NPTmax)	Mdevice[j].exclude=true;

										Mdevice[j].preambleSuccess=false;
										Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2));

										cntBackoff=0;
									}
								}
								else//not detect by BS
								{
									int randBackoff=lcgrand(7) * g_WBO;
									Mdevice[j].arrivalTime = ceil((i+randBackoff-g_t1+g_TRAR)/5);
									if(Mdevice[j].arrivalTime<=raSlot)	++Mdevice[j].arrivalTime;

									++Mdevice[j].nthPreamble;
									if(Mdevice[j].nthPreamble>g_NPTmax)	Mdevice[j].exclude=true;

									Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2));

								}
							}
						}
						if(cntRAR>0)	RAR.resize(cntRAR);
						else if(cntRAR==0)	RAR.clear();

						while(RAR.size()>15)//RAR[position]: device ID
						{
							int position =(int)lcgrand(10) * (RAR.size()-1);
							int randBackoff=lcgrand(7) * g_WBO;
							Mdevice[RAR[position]].arrivalTime=ceil((i+randBackoff-g_t1+g_TRAR+g_WRAR)/5);
							++Mdevice[RAR[position]].nthPreamble;
							if(Mdevice[RAR[position]].nthPreamble>g_NPTmax)	Mdevice[RAR[position]].exclude=true;
							Mdevice[RAR[position]].preamble = (int)((g_RAOs-1) * lcgrand(2));
							RAR.erase (RAR.begin()+position);
						}
						for (int j = 0; j < RAR.size(); ++j)
						{
							Mdevice[RAR[j]].preambleSuccess=true;
							// cout<<"RAR Success"<<endl;
							// cout <<"ra slot:"<<raSlot<<endl;
							// cout << "device ID:" << RAR[j]<<endl;
							// system("pause");
						}
						RAR.clear ();
						RAR.resize (20);
						cntRAR=0;
						++raSlot;
					}
					// system("pause");
					if((i-2)%5==0&&i>10)//12,17,22,27...59997 Msg start transmission
					{
						for (int j = 0; j < M[index]; ++j)//M[0]=5000 devices
						{
							// if(Mdevice[j].preambleSuccess=true&&!(Mdevice[j].msgSuccess)&&Mdevice[j].initArrivalTime<i)
							if(Mdevice[j].preambleSuccess&&!(Mdevice[j].msgSuccess)&&!(Mdevice[j].exclude))
							{
								if(lcgrand(7)>g_ProbErrMsg)//Msg3&4 success
								{
									Mdevice[j].msgSuccess=true;
									Mdevice[j].delay=i+21-(g_t1+Mdevice[j].initArrivalTime * 5);// 9 or 22
									// cout << "Timeslot: "<<i << endl;
									// cout <<"deviceID:"<<j<<endl;
									// cout << "initArrivalTime:"<<Mdevice[j].initArrivalTime<<endl;
									// cout << "delay:"<<Mdevice[j].delay << endl;
									// system("pause");
									// Mdevice[j].exclude=true;
								}
								else
								{
									int randBackoff=lcgrand(7) * g_WBO;
									Mdevice[j].arrivalTime = ceil((i+48+randBackoff-g_t1)/5);
									if(Mdevice[j].arrivalTime<=raSlot)	Mdevice[j].arrivalTime=raSlot;

									++Mdevice[j].nthPreamble;
									if(Mdevice[j].nthPreamble>g_NPTmax)	Mdevice[j].exclude=true;

									Mdevice[j].msgSuccess=false;
									Mdevice[j].preambleSuccess=false;
									Mdevice[j].preamble = (int)((g_RAOs-1) * lcgrand(2));
								}
							}
						}
					}
					// system("pause");
				}
				for (int i = 0; i < M[index]; ++i)
				{
					if(Mdevice[i].msgSuccess)
					{
						++successMS;
						tempDa = tempDa + Mdevice[i].delay;
					}
				}
				// system("pause");
				int cntNthPramble=0;
				int tempIndex=0;
				for (int i = 1; i <= g_NPTmax; ++i)
				{
					for (int j = 0; j < M[index]; ++j)
					{
						if(Mdevice[j].msgSuccess&&Mdevice[j].nthPreamble<=i)	++cntNthPramble;
					}
					simFm[tempIndex] = simFm[tempIndex] + cntNthPramble/(double)successMS;
					++tempIndex;
					// filePtrSim2AccumNthPreamble << i << "," << cntNthPramble/(double)successMS<<endl;
					cntNthPramble=0;
				}
				int cntAccessDelay=0;
				tempIndex=0;
				for (int i = 0; i <= 275; ++i)
				{
					for (int j = 0; j < M[index]; ++j)
					{
						if(Mdevice[j].msgSuccess&&Mdevice[j].delay<=i)	++cntAccessDelay;
					}
					simGd[tempIndex] = simGd[tempIndex] + cntAccessDelay/(double)successMS;
					++tempIndex;
					// filePtrSim2AccumAccessDelay << i << "," << cntAccessDelay/(double)successMS<<endl;
					cntAccessDelay=0;
				}
					// }
					simPc = simPc + collidedRAOs/(double)(simIR * g_RAOs);
					simPs = simPs + successMS/(double)M[index];
					// cout << successMS << M[index]<< endl;
					// return 0;
					simDa = simDa + tempDa/(double)successMS;
					// cout <<"collidedRAOs: "<<collidedRAOs<<"successMS:"<<successMS<<endl;
					// system("pause");
					// filePtrSimPcMode2 << M[index] << "," << simPc << endl;
					// filePtrSimPsMode2 << M[index] << "," << simPs << endl;
					// filePtrSimDaMode2 << M[index] << "," << simDa << endl;
					// system("pause");
					delete[] Mdevice;
			}//For Simulation Times:0-5
			for (int i = 0; i < g_NPTmax; ++i)
			{
				filePtrSim2AccumNthPreamble<<i<<","<<simFm[i]/(double)simTime<<endl;
			}
			for (int i = 0; i <= 275; ++i)
			{
				filePtrSim2AccumAccessDelay<<i<<","<<simGd[i]/(double)simTime<<endl;
			}
			simPc = simPc/(double)simTime;
			simPs = simPs/(double)simTime;
			simDa = simDa/(double)simTime;
			filePtrSimPcMode2 << M[index] << "," << simPc << endl;
			filePtrSimPsMode2 << M[index] << "," << simPs << endl;
			filePtrSimDaMode2 << M[index] << "," << simDa << endl;
		}//for index=0-2
		// filePtrSimArrMode2.close();
		filePtrSimPcMode2.close();
		filePtrSimPsMode2.close();
		filePtrSimDaMode2.close();
		filePtrSim2AccumNthPreamble.close();
		filePtrSim2AccumAccessDelay.close();
	}//traffic Model 2
	return 0;
}

long long int power(int a) // (64bits) :  -9223372036854775808 ~ 9223372036854775807
{
	long long int sum=1;
	for(int i=1; i<=a; i++)
	{
		sum=sum*i; //Math: (a!)(Math: a order)
	}
	return sum;
}

double ProbofBeta(int t, int period)//return 0-1 beta distribution probability
{
	int alpha=3;
    int beta=4;
	double betafunction=(double)power(alpha-1)*power(beta-1)/power(alpha+beta-1);
	return ( pow((double) t,(double) (alpha-1))*pow((double) (period-t),(double) (beta-1)) )/( pow((double) period,(double) (alpha+beta-1))*betafunction );
}

int GetDeltaTE(int index)
{
	if((g_Tp[index]-g_t1)%g_T_RA_REP==0)
		return 0;
	else
		return g_T_RA_REP-((g_Tp[index]-g_t1)%g_T_RA_REP);
}


void analColliProb()
{
	int TRAI1 = g_Tp[0]+GetDeltaTE(0) +g_TRAmax;
	int TRAI2 = g_Tp[1]+GetDeltaTE(1) +g_TRAmax;
	int IR1 = floor(TRAI1/g_T_RA_REP);
	int IR2 = floor(TRAI2/g_T_RA_REP);
	cout << IR1 << ',' << IR2 << endl;
}


void analArrivalMode2()
{
	ofstream filePtrArrMode2, filePtrArrMode2_CDF;
	filePtrArrMode2.open("AnalyArrivalMode2_M_5000_RA_Slot_1_2000.csv", ios::out);
	filePtrArrMode2_CDF.open("CDF_AnalyArrivalMode2_M_5000_RA_Slot_1_2000.csv", ios::out);
	int totalRaSlot = g_Tp[1]/g_T_RA_REP;//2000
	double accumNumArrMode2=0;
	vector<double> averNumArrMode2(totalRaSlot); // record arrival in each RA slot

 	for (int i = 0; i < totalRaSlot; ++i)
	{
		averNumArrMode2[i]=M[0]*ProbofBeta(i,totalRaSlot);
		accumNumArrMode2=accumNumArrMode2+averNumArrMode2[i];
		// cout << accumNumArrMode2 <<endl;
		// cout << averNumArrMode2[i] << endl;
		filePtrArrMode2 << i << "," << averNumArrMode2[i] << endl;
		filePtrArrMode2_CDF << i << "," << accumNumArrMode2 << endl;
	}
	filePtrArrMode2.close();
	filePtrArrMode2_CDF.close();
}

void arrivalMode2()
{
	ofstream filePtrSimArrMode2, filePtrSimArrMode2_CDF;
	filePtrSimArrMode2.open("SimArrivalMode2_M_5000_RA_Slot_1_2000.csv", ios::out);
	filePtrSimArrMode2_CDF.open("CDF_SimArrivalMode2_M_5000_RA_Slot_1_2000.csv", ios::out);
	int sim2RaSlot=g_Tp[1]/g_T_RA_REP;;// g_Tp[1]/g_T_RA_REP=2000
	int simAccumArrMode2=0, SimTimes2=10000;
	vector<int> cdfArrivalNum2(sim2RaSlot+200), ArrivalNum2(sim2RaSlot+200);
	// ArrivalNum2 = new int[sim2RaSlot+200)]();
	// system("pause");
	int simCDFTimes2=SimTimes2-9900;
	for(int i=0;i<simCDFTimes2;++i)
	{
		for (int j = 0; j < sim2RaSlot; j++)
		{
			double betaCDFforithRAslot=ProbofBeta(j,sim2RaSlot);
			for(int k=0;k<M[0];k++)
			{
				if(lcgrand(5)<=betaCDFforithRAslot)	cdfArrivalNum2[j]=cdfArrivalNum2[j]+1;
			}
		}
	}


	for(int i=0;i<SimTimes2;++i)
	{
		for (int j = 0; j < sim2RaSlot; j=j+50)
		{
			double betaPDFforithRAslot=ProbofBeta(j,sim2RaSlot);
			for(int k=0;k<M[0];k++)
			{
				if(lcgrand(5)<=betaPDFforithRAslot)	ArrivalNum2[j]=ArrivalNum2[j]+1;
			}
		}
	}


	for(int i=sim2RaSlot;i<sim2RaSlot+200;++i)
	{
		ArrivalNum2[i]=0;
		cdfArrivalNum2[i]=0;
	}

	for (int i = 0; i < sim2RaSlot+200; i=i+50)	filePtrSimArrMode2 << i << "," << (double)ArrivalNum2[i]/(double)SimTimes2 << endl;
	// system("pause");
	for(int i=0;i<sim2RaSlot+200;i++)
	{
		simAccumArrMode2=simAccumArrMode2+cdfArrivalNum2[i];
		filePtrSimArrMode2_CDF << i << "," << (double)simAccumArrMode2/(double)simCDFTimes2 << endl;
	}
	// delete [] ArrivalNum2;
	filePtrSimArrMode2.close();
	filePtrSimArrMode2_CDF.close();
}

void analArrivalMode1()
{
	ofstream filePtrArrMode1, filePtrArrMode1_CDF;
	int leftNumM=M[0],cdfIndex=0;
	double accumNumArrMS=0;
	int AnalRaSlot=g_Tp[0]/g_T_RA_REP;// Traffic Model1 RA slot
	double *averNumArrMS;
	averNumArrMS = new double[AnalRaSlot]();
	filePtrArrMode1.open("AnalyArrivalMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
	filePtrArrMode1_CDF.open("CDF_AnalyArrivalMode1_M_5000_RA_Slot_1_12000.csv", ios::out);
	for (int i = 0;i<AnalRaSlot; ++i)
	{
		averNumArrMS[i]=(double)M[0]/(double)AnalRaSlot;
		filePtrArrMode1 << i << "," << averNumArrMS[i] << endl;
	}
	// cout << cdfIndex;
	for(int i = 0;i<AnalRaSlot;++i)
	{
		accumNumArrMS=accumNumArrMS+averNumArrMS[i];
		filePtrArrMode1_CDF << i << "," << accumNumArrMS << endl;
	}
	delete [] averNumArrMS;
	filePtrArrMode1.close();
	filePtrArrMode1_CDF.close();
}
