//
// Copyright (C) 2006 Andras Babos and Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#include "LSA.h"


bool OSPF::SummaryLSA::update(const OSPFSummaryLSA* lsa)
{
    bool different = differsFrom(lsa);
    (*this) = (*lsa);
    resetInstallTime();
    if (different) {
        clearNextHops();
        return true;
    } else {
        return false;
    }
}

bool OSPF::SummaryLSA::differsFrom(const OSPFSummaryLSA* summaryLSA) const
{
    const OSPFLSAHeader& lsaHeader = summaryLSA->getHeader();
    bool differentHeader = ((header_var.getLsOptions() != lsaHeader.getLsOptions()) ||
                            ((header_var.getLsAge() == MAX_AGE) && (lsaHeader.getLsAge() != MAX_AGE)) ||
                            ((header_var.getLsAge() != MAX_AGE) && (lsaHeader.getLsAge() == MAX_AGE)) ||
                            (header_var.getLsaLength() != lsaHeader.getLsaLength()));
    bool differentBody = false;

    if (!differentHeader) {
        differentBody = ((networkMask_var != summaryLSA->getNetworkMask()) ||
                         (routeCost_var != summaryLSA->getRouteCost()) ||
                         (tosData_arraysize != summaryLSA->getTosDataArraySize()));

        if (!differentBody) {
            unsigned int tosCount = tosData_arraysize;
            for (unsigned int i = 0; i < tosCount; i++) {
                if ((tosData_var[i].tos != summaryLSA->getTosData(i).tos) ||
                    (tosData_var[i].tosMetric[0] != summaryLSA->getTosData(i).tosMetric[0]) ||
                    (tosData_var[i].tosMetric[1] != summaryLSA->getTosData(i).tosMetric[1]) ||
                    (tosData_var[i].tosMetric[2] != summaryLSA->getTosData(i).tosMetric[2]))
                {
                    differentBody = true;
                    break;
                }
            }
        }
    }

    return (differentHeader || differentBody);
}
