/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*

 Copyright (C) 2008 Mark Joshi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/


#ifndef quantlib_pathwise_accounting_engine_hpp
#define quantlib_pathwise_accounting_engine_hpp

#include <ql/models/marketmodels/pathwisemultiproduct.hpp>
#include <ql/models/marketmodels/pathwisediscounter.hpp>
#include <ql/math/statistics/sequencestatistics.hpp>

#include <ql/utilities/clone.hpp>
#include <ql/types.hpp>
#include <vector>

namespace QuantLib {

    class LogNormalFwdRateEuler;
    class MarketModel;


    //! Engine collecting cash flows along a market-model simulation for doing pathwise computation of Deltas
    // using Giles--Glasserman smoking adjoints method
    // note only works with displaced LMM, and requires knowledge of pseudo-roots and displacements 
    class PathwiseAccountingEngine 
    {
      public:
        PathwiseAccountingEngine(const boost::shared_ptr<LogNormalFwdRateEuler>& evolver, // method relies heavily on LMM Euler
                         const Clone<MarketModelPathwiseMultiProduct>& product,
                         const boost::shared_ptr<MarketModel>& pseudoRootStructure, // we need pseudo-roots and displacements
                         Real initialNumeraireValue);

        void multiplePathValues(SequenceStatistics& stats,
                                Size numberOfPaths);
      private:
          Real singlePathValues(std::vector<Real>& values);

        boost::shared_ptr<LogNormalFwdRateEuler> evolver_;
        Clone<MarketModelPathwiseMultiProduct> product_;
        boost::shared_ptr<MarketModel> pseudoRootStructure_;

        Real initialNumeraireValue_;
        Size numberProducts_;
        Size numberRates_;
        Size numberCashFlowTimes_;
        Size numberSteps_;

        std::vector<Real> currentForwards_, lastForwards_;

        bool doDeflation_;


        // workspace
        std::vector<Real> numerairesHeld_;
        std::vector<Size> numberCashFlowsThisStep_;
        std::vector<std::vector<MarketModelPathwiseMultiProduct::CashFlow> >
                                                         cashFlowsGenerated_;
        std::vector<MarketModelPathwiseDiscounter> discounters_;

        std::vector<Matrix> V_;  // one V for each product, with components for each time step and rate

   //     std::vector<std::vector<std::vector<Real> > > V_; // one V for each product, with components for each time step and rate

        Matrix LIBORRatios_; // dimensions are step and rate number
        Matrix Discounts_; // dimensions are step and rate number, goes from 0 to n. P(t_0, t_j)

        Matrix StepsDiscountsSquared_; // dimensions are step and rate number

        Matrix LIBORRates_; // dimensions are step and rate number
        Matrix partials_; // dimensions are factor and rate

        std::vector<Real> deflatorAndDerivatives_;
        
        std::vector<std::vector<Size> > numberCashFlowsThisIndex_;
        std::vector<Matrix> totalCashFlowsThisIndex_; // need product cross times cross which sensitivity

        std::vector<std::vector<Size> > cashFlowIndicesThisStep_;

    };

}

#endif
