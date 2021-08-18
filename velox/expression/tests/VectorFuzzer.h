/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <folly/Random.h>

#include "velox/type/Type.h"
#include "velox/vector/BaseVector.h"

namespace facebook::velox {

// Helper class to generate randomized vectors with random (and potentially
// nested) encodings. Use the constructor seed to make it deterministic.
class VectorFuzzer {
 public:
  struct Options {
    size_t vectorSize{100};

    // One in X chance of generating a null value in the output vector (e.g: 10
    // for 10%, 2 for 50%, 1 for 100%, 0 for 0%).
    size_t nullChance{0};

    // Size of the generated strings. If `stringVariableLength` is true, the
    // semantic of this option becomes "string maximum length".
    size_t stringLength{50};

    // Whether generated strings should include UTF8 characters.
    bool stringUtf8{false};

    // If true, the length of strings are randomly generated and `stringLength`
    // is treated as maximum length.
    bool stringVariableLength{false};
  };

  VectorFuzzer(
      VectorFuzzer::Options options,
      memory::MemoryPool* pool,
      size_t seed = 123456)
      : opts_(options), pool_(pool), rng_(seed) {}

  // Returns a "fuzzed" vector, containing randomized data, nulls, and indices
  // vector (dictionary).
  VectorPtr fuzz(const TypePtr& type);

  // Returns a flat vector with randomized data and nulls.
  VectorPtr fuzzFlat(const TypePtr& type);

  // Wraps `vector` using a randomized indices vector, returning a
  // DictionaryVector.
  VectorPtr fuzzDictionary(const VectorPtr& vector);

 private:
  // Returns true 1/n of times.
  bool oneIn(size_t n) {
    return folly::Random::oneIn(n, rng_);
  }

  const VectorFuzzer::Options opts_;

  memory::MemoryPool* pool_;

  folly::Random::DefaultGenerator rng_;
};

} // namespace facebook::velox
