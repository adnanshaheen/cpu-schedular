
#pragma once

/**
 * Header file
 */
#include <queue>

/**
 * CJob class
 * 
 * job information
 */
class CJob
{
	unsigned int m_nJob;		/* Job number */
	unsigned int m_nBurst;		/* Burst time */
	unsigned int m_nArrival;	/* Arrival time */
	unsigned int m_nType;		/* Type of CPU scheduling */
	unsigned int m_nTime;		/* Total time spent unless terminated */
	unsigned int m_nRunning;	/* Total running time */

public:
	/* Constructor/Destructor */
	CJob() {};
	CJob(unsigned int nType, unsigned int nJob, unsigned int nArrival, unsigned int nBurst);
	CJob(const CJob& cJob);		/* Copy constructor */
	~CJob() {};

	/**
	 * SetType:
	 * nType: constant number to provide information on type of scheduling
	 *        and job information to be read from file or created at random
	 */
	inline void SetType(const unsigned int nType)
	{
		m_nType = nType;
	}
	/**
	 * GetType:
	 * Return type of scheduling information
	 */
	inline unsigned int GetType() const
	{
		return m_nType;
	}
	/**
	 * SetJob:
	 * nJob: Job number information
	 */
	inline void SetJob(const unsigned int nJob)
	{
		m_nJob = nJob;
	}
	/**
	 * GetJob:
	 * Returns the job number
	 */
	inline unsigned int GetJob() const
	{
		return m_nJob;
	}
	/**
	 * SetBurst:
	 * nBurst: Burst time for a job
	 */
	inline void SetBurst(const unsigned int nBurst)
	{
		m_nBurst = nBurst;
	}
	/**
	 * GetBurst:
	 * Returns the burst time for this job
	 */
	inline unsigned int GetBurst() const
	{
		return m_nBurst;
	}
	/**
	 * SetArrival:
	 * nArrival: Sets the arrival time for this job
	 */
	inline void SetArrival(const unsigned int nArrival)
	{
		m_nArrival = nArrival;
	}
	/**
	 * GetArrival:
	 * Returns the arrival time for job
	 */
	inline unsigned int GetArrival() const
	{
		return m_nArrival;
	}
	/**
	 * SetTime:
	 * nTime: Sets the total time for this job
	 */
	inline void SetTime(const unsigned int nTime)
	{
		m_nTime = nTime;
	}
	/**
	 * GetTime:
	 * Returns the total time for this job
	 */
	inline unsigned int GetTime() const
	{
		return m_nTime;
	}
	/**
	 * SetRunning:
	 * nRunning: Sets the running time for this job
	 */
	inline void SetRunning(const unsigned int nRunning)
	{
		m_nRunning = nRunning;
	}
	/**
	 * GetRunning:
	 * Returns the current running time
	 */
	inline unsigned int GetRunning() const
	{
		return m_nRunning;
	}

	/**
	 * Comparitors for priority_queue
	 */
	friend bool operator < (const CJob& lhs, const CJob& rhs);
	friend bool operator > (const CJob& lhs, const CJob& rhs);
};

/**
 * CSchedular class
 * 
 * Main scheduling class
 */
class CSchedular
{
public:
	/* Constructor/Desctuctor */
	CSchedular(unsigned int nType, unsigned int nTimeQuantum, char* pFileName, unsigned int nJobs, bool bVerbose);
	~CSchedular() {};

	/**
	 * SetTime:
	 * nTime: Set total time for all jobs
	 */
	inline void SetTime(unsigned int nTime)
	{
		m_nTime = nTime;
	}
	/**
	 * GetTime:
	 * Returns the current time for all the jobs
	 */
	inline unsigned int GetTime() const
	{
		return m_nTime;
	}
	/**
	 * IsFIFO:
	 * Is scheduling First come first serve?
	 */
	inline bool IsFIFO() const
	{
		return m_nType & FIFO;
	}
	/**
	 * IsSRJF:
	 * Is scheduling Shortest Remaining job first (preemptive)?
	 */
	inline bool IsSRJF() const
	{
		return m_nType & SRJF;
	}
	/**
	 * IsRoundRobin:
	 * Is scheduling Round Robin?
	 */
	inline bool IsRoundRobin() const
	{
		return m_nType & RR;
	}
	/**
	 * IsRandom:
	 * Are we making random jobs?
	 */
	inline bool IsRandom() const
	{
		return m_nType & RAND;
	}
	/**
	 * SetTimeQuantum:
	 * nTimeQuantum: Set the time quantum for round robin scheduling
	 */
	inline void SetTimeQuantum(unsigned int nTimeQuantum)
	{
		m_nTimeQuantum = nTimeQuantum;
	}
	/**
	 * GetTimeQuantum:
	 * Returns the time quantum for round robin scheduling
	 */
	inline unsigned int GetTimeQuantum() const
	{
		return m_nTimeQuantum;
	}

	/**
	 * Start:
	 * Main entry point in class
	 */
	int Start();

private:
	int ReadFile();		/* Read jobs from file */
	int Random();		/* Create random jobs */
	int Execute();		/* Execute the scheduling algorithm */
	void Clear();		/* Clear the data structures */
	int DisplayJobs();	/* Display the jobs (debugging purpose) */
	int ExecuteFCFS();	/* Execute the FCFS algorithm */
	int ExecuteSRJF();	/* Execute the SRJF algorithm */
	int ExecuteRR();	/* Execute the round robin algorithm */

private:
	unsigned int m_nType;		/* Type of scheduling */
	unsigned int m_nTime;		/* Total time for jobs */
	char* m_pFileName;		/* File name of file to read data from */
	std::priority_queue<CJob> m_cQueue;	/* Priority Q, for FCFS, and SRJF */
	std::queue<CJob> m_cRRQueue;	/* Q for round robin, can't use Priority Q (because Q.top() is constant) */
	std::vector<CJob> m_cList;	/* List of all jobs */
	bool m_bVerbose;		/* verbose mode output */
	unsigned int m_nTimeQuantum;	/* Time quantum for round robin scheduling */
	unsigned int m_nJobs;		/* Number of jobs in case of random jobs */
};
