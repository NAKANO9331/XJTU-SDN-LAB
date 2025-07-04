/*
 * EquivalenceClass.h
 *
 *  Created on: Mar 15, 2012
 *      Author: khurshid
 *
 * This file is protected by the VeriFlow Research License Agreement
 * available at http://www.cs.illinois.edu/~khurshi1/projects/veriflow/veriflow-research-license-agreement.txt.
 * A copy of this agreement is also included in this package.
 *
 * Copyright (c) 2012-2013 by
 * The Board of Trustees of the University of Illinois.
 * All rights reserved.
 */

#ifndef EQUIVALENCECLASS_H_
#define EQUIVALENCECLASS_H_

#include <sys/types.h>
 #include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <string>

using namespace std;

enum FieldIndex
{
	IN_PORT, // 0
	DL_SRC,
	DL_DST,
	DL_TYPE,
	DL_VLAN,
	DL_VLAN_PCP,
	MPLS_LABEL,
	MPLS_TC,
	NW_SRC,
	NW_DST,
	NW_PROTO,
	NW_TOS,
	TP_SRC,
	TP_DST,
	ALL_FIELD_INDEX_END_MARKER, // 14
	METADATA, // 15, not used in this version.
	WILDCARDS // 16
};

const unsigned int fieldWidth[] = {16, 48, 48, 16, 12, 3, 20, 3, 32, 32, 8, 6, 16, 16, 0, 64, 32};

class EquivalenceClass
{
public:
	uint64_t lowerBound[ALL_FIELD_INDEX_END_MARKER], upperBound[ALL_FIELD_INDEX_END_MARKER];

	EquivalenceClass();
	EquivalenceClass(const uint64_t* lb, const uint64_t* ub);
	uint64_t getRange(int fieldIndex) const;
	bool equals(const EquivalenceClass& other) const;
	bool operator==(const EquivalenceClass& other) const;
	int operator()() const;
	bool subsumes(const EquivalenceClass &other) const;
	void clear();
	string toString() const;

	static uint64_t getMaxValue(FieldIndex index);
	EquivalenceClass intersection(const EquivalenceClass& other) const;
    	EquivalenceClass difference(const EquivalenceClass& other) const;
    	bool isEmpty() const;
    	
    	// 辅助方法，用于输出 EquivalenceClass 的信息
   	// 辅助方法，用于输出 EquivalenceClass 的信息
    	void printInfo(const std::string& prefix) const {
        std::cout << prefix << ": ";
        for (int i = 0; i < ALL_FIELD_INDEX_END_MARKER; ++i) {
            std::cout << "[" << lowerBound[i] << ", " << upperBound[i] << "] ";
        }
        std::cout << std::endl;
    }
};


#endif /* EQUIVALENCECLASS_H_ */
