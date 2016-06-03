// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.

// A frontend to the different analyzers in logle.
#ifndef LOGLE_FRONTEND_H_
#define LOGLE_FRONTEND_H_

#include "analysis_options.pb.h"
#include "util/status.h"

namespace logle {
namespace frontend {

enum class Analyzer { kCurio, kMailAccess, kPlaso };

// Returns the status generated by running the analyzer in the first argument
// with the options in the second argument. Error codes are generated in the
// following situations.
// * The input format is not supported by the analyzer.
//  - The Curio analyzer requires JSON input.
//  - The MailAccess analyzer requires CSV input.
// * File I/O errors when reading from an input file, writing to the output DOT
//   file, or closing either the input or output file.
// * Analyzer generated errors. Consult the individual analyzer documentation
//   for situations in which the analyzers return errors.
util::Status Run(Analyzer analyzer, const AnalysisOptions& options);

}  // namespace frontend
}  // logle
#endif  // LOGLE_FRONTEND_H_
