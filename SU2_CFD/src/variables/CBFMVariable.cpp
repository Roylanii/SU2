/*!
 * \file CBFMVariable.cpp
 * \brief Definition of the BFM solution fields.
 * \author E.C.Bunschoten
 * \version 7.1.0 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2020, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */


#include "../../include/variables/CBFMVariable.hpp"
#include "../../../Common/include/parallelization/omp_structure.hpp"

CBFMVariable::CBFMVariable(unsigned long nPoints, unsigned short NDim, unsigned short nBFMParams){
    nPoint = nPoints;
    nDim = NDim;
    nAuxVar = nBFMParams;

    
    AuxVar.resize(nPoint, nAuxVar) = su2double(0.0);
    Body_Force_Factor.resize(nPoint);
    Body_Force_Vector.resize(nPoint);
    Relative_Velocity.resize(nPoint);
    proj_vector_axial.resize(nPoint);
    proj_vector_tangential.resize(nPoint);
    proj_vector_radial.resize(nPoint);
    for(unsigned long iPoint=0; iPoint<nPoint; ++iPoint){
        Body_Force_Vector.at(iPoint).resize(nDim);
        Relative_Velocity.at(iPoint).resize(nDim);
        proj_vector_axial.at(iPoint).resize(nDim);
        proj_vector_tangential.at(iPoint).resize(nDim);
        proj_vector_radial.at(iPoint).resize(nDim);
    }
    Grad_AuxVar.resize(nPoint, nAuxVar, nDim, 0.0);
    Solution.resize(nPoint, nBFMParams) = su2double(0.0);
    Gradient.resize(nPoint, nBFMParams, nDim, 0.0);
    Blockage_Gradient.resize(nPoint, nDim) = su2double(0.0);
    
};

void CBFMVariable::SizeParameters(unsigned long nPoints, unsigned short nDim){};

