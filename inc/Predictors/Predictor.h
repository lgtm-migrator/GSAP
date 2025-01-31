// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PREDICTOR_H
#define PCOE_PREDICTOR_H

#include <string>
#include <vector>

#include "CompositeSavePointProvider.h"
#include "ConfigMap.h"
#include "Contracts.h"
#include "DataPoint.h"
#include "Loading/LoadEstimator.h"
#include "Models/PrognosticsModel.h"
#include "ProgEvent.h"
#include "Trajectory/TrajectoryService.h"

namespace PCOE {
    class Prediction {
    public:
        Prediction(std::vector<ProgEvent> events, std::vector<DataPoint> observables)
            : events(std::move(events)), observables(std::move(observables)) {}
		
        static Prediction & EmptyPrediction() {
            static Prediction emptyPrediction({},{});
            return emptyPrediction;
	    }

        inline const std::vector<ProgEvent>& getEvents() const {
            return events;
        }

        inline const std::vector<DataPoint>& getObservables() const {
            return observables;
        }

    private:
	    std::vector<ProgEvent> events;
	    std::vector<DataPoint> observables;
    };

    /**
     * Represents a model-based predictor.
     *
     * @author Matthew Daigle
     * @author Jason Watkins
     * @author Chris Teubert
     **/
    class Predictor {
    public:
        /**
         * Constructs a new predictor using configuration options specified in
         * the given config map.
         *
         * @param m      The model used by the predictor.
         * @param le     The load estimator used by the predictor.
         * @param ts     The trajectory service used by the predictor
         * @param config A config map containing configuration parameters for
         *               the predictor.
         **/
        Predictor(const PrognosticsModel& m,
                  LoadEstimator& le,
                  const TrajectoryService& ts,
                  const ConfigMap& config)
            : loadEstimator(le), model(m), trajService(ts) {
            // Note (JW):
            // Want to keep config param in case we ever add optional config values
            // This cast suppresses warnings about the unused variable.
            static_cast<void>(config);
            savePointProvider.add(&trajService);
        }

        /**
         * Default destructor. A virtual default destructor is necessary to
         * ensure that resources in classes inherting from this class are
         * cleaned up, even if the destructor is called through a pointer of the
         * base class type.
         **/
        virtual ~Predictor() = default;

        /**
         * Predict future events and values of system variables
         *
         * @param t     Time of prediction
         * @param state State of system at time of prediction
         **/
        virtual Prediction predict(double t, const std::vector<UData>& state) = 0;

        /**
         * Gets a list of the observables predicted by the
         * current predictor.
         **/
        inline const std::vector<std::string>& getObservableNames() {
            return observables;
        }

    protected:
        /**
         * Sets the outputs (system trajectories) predicted by the current
         * predictor.
         **/
        inline void setObservableNames(std::vector<std::string> value) {
            observables = std::move(value);
        }

        LoadEstimator& loadEstimator;
        const PrognosticsModel& model;
        CompositeSavePointProvider savePointProvider;
        const TrajectoryService& trajService;

    private:
        std::vector<std::string> observables;
    };
}

#endif
