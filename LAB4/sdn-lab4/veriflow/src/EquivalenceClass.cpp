/*
 * EquivalenceClass.cpp
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

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "EquivalenceClass.h"
#include "VeriFlow.h"

using namespace std;

void swapBounds(uint64_t& lower, uint64_t& upper) {
    uint64_t temp = lower;
    lower = upper;
    upper = temp;
}

EquivalenceClass::EquivalenceClass()
{
    this->clear();
}

EquivalenceClass::EquivalenceClass(const uint64_t* lb, const uint64_t* ub)
{
    for(int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++)
    {
        if(lb[i] > ub[i])
        {
            swapBounds(const_cast<uint64_t&>(lb[i]), const_cast<uint64_t&>(ub[i]));
        }

        this->lowerBound[i] = lb[i];
        this->upperBound[i] = ub[i];
    }
}

uint64_t EquivalenceClass::getRange(int fieldIndex) const
{
    return (this->upperBound[fieldIndex] - this->lowerBound[fieldIndex]);
}

bool EquivalenceClass::equals(const EquivalenceClass& other) const
{
    for(int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++)
    {
        if((this->lowerBound[i] != other.lowerBound[i]) || (this->upperBound[i] != other.upperBound[i]))
        {
            return false;
        }
    }

    return true;
}

bool EquivalenceClass::operator==(const EquivalenceClass& other) const
{
    return this->equals(other);
}

bool EquivalenceClass::subsumes(const EquivalenceClass &other) const
{
    for(int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++)
    {
        if((this->lowerBound[i] <= other.lowerBound[i] && this->upperBound[i] >= other.upperBound[i]))
        {
        } else {
            return false;
        }
    }

    return true;

}

int EquivalenceClass::operator()() const
{
    int retVal = 0;
    for(int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++)
    {
        retVal += (this->lowerBound[i] + this->upperBound[i]);
    }

    return retVal;
}

void EquivalenceClass::clear()
{
    for(int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++)
    {
        this->lowerBound[i] = 0;
        this->upperBound[i] = 0;
    }
}

string EquivalenceClass::toString() const
{
    char buffer[1024];
    sprintf(buffer, "[EquivalenceClass] dl_src (%lu-%s, %lu-%s), dl_dst (%lu-%s, %lu-%s)",
            this->lowerBound[DL_SRC], ::getMacValueAsString(this->lowerBound[DL_SRC]).c_str(),
            this->upperBound[DL_SRC], ::getMacValueAsString(this->upperBound[DL_SRC]).c_str(),
            this->lowerBound[DL_DST], ::getMacValueAsString(this->lowerBound[DL_DST]).c_str(),
            this->upperBound[DL_DST], ::getMacValueAsString(this->upperBound[DL_DST]).c_str());

    string retVal = buffer;
    retVal += ", ";

    sprintf(buffer, "nw_src (%lu-%s, %lu-%s), nw_dst (%lu-%s, %lu-%s)",
            this->lowerBound[NW_SRC], ::getIpValueAsString(this->lowerBound[NW_SRC]).c_str(),
            this->upperBound[NW_SRC], ::getIpValueAsString(this->upperBound[NW_SRC]).c_str(),
            this->lowerBound[NW_DST], ::getIpValueAsString(this->lowerBound[NW_DST]).c_str(),
            this->upperBound[NW_DST], ::getIpValueAsString(this->upperBound[NW_DST]).c_str());

    retVal += buffer;
    retVal += ", ";

    for(int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++)
    {
        sprintf(buffer, "Field %d (%lu, %lu)", i, this->lowerBound[i], this->upperBound[i]);
        retVal += buffer;

        if(i < (ALL_FIELD_INDEX_END_MARKER - 1))
        {
            retVal += ", ";
        }
    }

    return retVal;
}

uint64_t EquivalenceClass::getMaxValue(FieldIndex index)
{
    switch(index)
    {
    case IN_PORT:
        return 0xFFFF;
        break;

    case DL_SRC:
        return 0xFFFFFFFFFFFFLLU;
        break;

    case DL_DST:
        return 0xFFFFFFFFFFFFLLU;
        break;

    case DL_TYPE:
        return 0xFFFF;
        break;

    case DL_VLAN:
        return 0xFFF;
        break;

    case DL_VLAN_PCP:
        return 0x7;
        break;

    case MPLS_LABEL:
        return 0xFFFFF;
        break;

    case MPLS_TC:
        return 0x7;
        break;

    case NW_SRC:
        return 0xFFFFFFFF;
        break;

    case NW_DST:
        return 0xFFFFFFFF;
        break;

    case NW_PROTO:
        return 0xFF;
        break;

    case NW_TOS:
        return 0x3F;
        break;

    case TP_SRC:
        return 0xFFFF;
        break;

    case TP_DST:
        return 0xFFFF;
        break;

    case METADATA:
        return 0xFFFFFFFFFFFFFFFFLLU;
        break;

    default:
        fprintf(stderr, "[EquivalenceClass::getMaxValue] Wrong field index (%d) specified. Terminating process.\n", index);
        exit(1);
        return 0x0; // Something went wrong; wrong field index was specified. Unreachable code due to the presence of exit(1).
        break;
    }
}

EquivalenceClass EquivalenceClass::intersection(const EquivalenceClass& other) const {
    uint64_t newLowerBound[ALL_FIELD_INDEX_END_MARKER];
    uint64_t newUpperBound[ALL_FIELD_INDEX_END_MARKER];

    bool allEmpty = true;
    for (int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++) {
        newLowerBound[i] = std::max(this->lowerBound[i], other.lowerBound[i]);
        newUpperBound[i] = std::min(this->upperBound[i], other.upperBound[i]);

        //if (newLowerBound[i] > newUpperBound[i]) {
            //uint64_t temp = newLowerBound[i];
            //newLowerBound[i] = newUpperBound[i];
            //newUpperBound[i] = temp;
        //}

        if (newLowerBound[i] == 0 && newUpperBound[i] == 0) {
            continue;
        } else {
            allEmpty = false;
        }
    }

    if (allEmpty) {
        return EquivalenceClass();
    }

    return EquivalenceClass(newLowerBound, newUpperBound);
}

EquivalenceClass EquivalenceClass::difference(const EquivalenceClass& other) const {
    std::vector<EquivalenceClass> resultParts;

    const EquivalenceClass& intersectionResult = other;
    for (int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++) {
        if (intersectionResult.lowerBound[i] <= intersectionResult.upperBound[i]) {

            if (this->lowerBound[i] < intersectionResult.lowerBound[i]) {
                EquivalenceClass part = *this;
                part.upperBound[i] = intersectionResult.lowerBound[i] - 1;
                resultParts.push_back(part);
            }
 
            if (this->upperBound[i] > intersectionResult.upperBound[i]) {
                EquivalenceClass part = *this;
                part.lowerBound[i] = intersectionResult.upperBound[i] + 1;
                resultParts.push_back(part);
            }
        }
    }

    if (!resultParts.empty()) {

        for (int field = 0; field < ALL_FIELD_INDEX_END_MARKER; field++) {
            std::vector<EquivalenceClass> newResultParts;
            while (!resultParts.empty()) {
                EquivalenceClass current = resultParts.back();
                resultParts.pop_back();
                bool merged = false;
                for (auto& part : newResultParts) {
                    if (current.lowerBound[field] <= part.upperBound[field] &&
                        current.upperBound[field] >= part.lowerBound[field]) {
                        part.lowerBound[field] = std::min(current.lowerBound[field], part.lowerBound[field]);
                        part.upperBound[field] = std::max(current.upperBound[field], part.upperBound[field]);
                        merged = true;
                        break;
                    }
                }
                if (!merged) {
                    newResultParts.push_back(current);
                }
            }
            resultParts = newResultParts;
        }

        EquivalenceClass mergedResult = resultParts[0];
        for (size_t j = 1; j < resultParts.size(); ++j) {
            for (int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++) {
                mergedResult.lowerBound[i] = std::min(mergedResult.lowerBound[i], resultParts[j].lowerBound[i]);
                mergedResult.upperBound[i] = std::max(mergedResult.upperBound[i], resultParts[j].upperBound[i]);
            }
        }
        for (int i = 0; i < ALL_FIELD_INDEX_END_MARKER; i++) {
            if (mergedResult.lowerBound[i] > mergedResult.upperBound[i]) {
                uint64_t temp = mergedResult.lowerBound[i];
                mergedResult.lowerBound[i] = mergedResult.upperBound[i];
                mergedResult.upperBound[i] = temp;
            }
        }

        return mergedResult;
    } else {
        return EquivalenceClass();
    }
}    

bool EquivalenceClass::isEmpty() const {
    for (int i = 0; i < ALL_FIELD_INDEX_END_MARKER; ++i) {
        if (this->lowerBound[i] != 0 || this->upperBound[i] != 0) {
            return false;
        }
    }
    return true;
}
