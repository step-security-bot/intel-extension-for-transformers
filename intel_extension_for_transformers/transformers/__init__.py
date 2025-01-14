#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2021 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


from .config import (
    WEIGHTS_NAME,
    BenchmarkConfig,
    DistillationConfig,
    DynamicLengthConfig,
    Provider,
    PrunerV2,
    PruningConfig,
    QuantizationConfig,
    TFDistillationConfig,
)
from .distillation import (
    SUPPORTED_DISTILLATION_CRITERION_MODE,
    DistillationCriterionMode,
)
from .optimizer import NoTrainerOptimizer, Orchestrate_optimizer
from .optimizer_tf import TFOptimization
from .pruning import SUPPORTED_PRUNING_MODE, PrunerConfig, PruningMode
from .quantization import SUPPORTED_QUANT_MODE, QuantizationMode
from .utils import (
    MixedPrecisionConfig,
    BitsAndBytesConfig,
    SmoothQuantConfig,
    StaticQuantConfig,
    DynamicQuantConfig,
    QuantAwareTrainingConfig,
    RtnConfig,
    AwqConfig,
    TeqConfig,
    GPTQConfig,
    AutoRoundConfig,
    metrics,
    objectives,
)
from .utils.utility import LazyImport
from .modeling import (
    AutoModelForCausalLM,
    AutoModel,
    AutoModelForSeq2SeqLM,
    OptimizedModel
)
