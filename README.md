# Gauss-Newton Method for Nonlinear Parameter Estimation and Model Fitting

C++ project for fitting nonlinear models with Gauss-Newton and Newton solvers. Includes a CLI test runner and a natID GUI with plots.

**Professor** — Izudin Dzafic  
**Student** — Almir Mustafic

## Project Background

The project estimates model parameters by minimizing a weighted least-squares residual. It implements three synthetic datasets, two solvers, batch experiments, CSV exports, and a GUI for interactive runs.

## Technologies

- **C++14** — core implementation
- **CMake** — build system
- **natID** — GUI framework (`natGUI`, `natPlot`) and matrix library (`Matrix`)
- **DevEnv** — CMake modules at `~/Work/DevEnv`

## Prerequisites

- macOS arm64 with Xcode/clang
- natID DevEnv at `~/Work/DevEnv` (`Common.cmake`, `natGUI.cmake`, `MatrixLib.cmake`)
- natID libraries at `~/other_bin/myLib` (`mainUtils`, `Matrix`, `natGUI`, `natPlot`)
- headers at `~/Work/Common/Include`

## Build

```bash
cd Implementation
mkdir -p build && cd build
cmake .. && make
```

Targets built by `make`

- `gauss_newton_cli` — dataset generation, solver tests, experiment exports
- `GaussNewtonFit` — GUI application bundle `GaussNewtonFit.app`

DevEnv also places `gauss_newton_cli` at `~/Projects/Out/GaussNewtonFit/gauss_newton_cli`.

## Run CLI

```bash
cd /build
./gauss_newton_cli
```

The CLI prints sample rows, runs model and solver tests, runs experiment and comparison tests, and writes CSV files in the current working directory.

## Run GUI

```bash
cd /build
make GaussNewtonFit
open GaussNewtonFit.app
```

## Models

| Model | Parameters | Default data range |
|-------|------------|-------------------|
| exponential | 2 | t in [0, 5], 10 points |
| logistic | 3 | t in [0, 5], 10 points |
| sinusoidal | 3 | t in [0, 5], 10 points |

Synthetic data uses a fixed seed (`42`) and configurable noise sigma.

## Solvers

- **Gauss-Newton** — uses the Gauss-Newton approximation of the Hessian
- **Newton** — uses the full Hessian from second derivatives of residuals

Both solvers share stopping rules based on `maxIterationCount`, `parameterTolerance`, and `residualTolerance` (`GaussNewtonConfig`).

## CLI Tests

`gauss_newton_cli` runs these test groups in order

1. model tests (Jacobian and Hessian checks)
2. Gauss-Newton solver tests
3. Newton solver tests
4. experiment runner tests
5. comparison tests (Gauss-Newton vs Newton)
6. report export tests

Exit code is `0` when all tests pass and `1` otherwise.

## CLI Output Files

| File | Content |
|------|---------|
| `exponential.csv` | exponential dataset |
| `logistic.csv` | logistic dataset |
| `sinusoidal.csv` | sinusoidal dataset |
| `comparison_results.csv` | per-run experiment metrics |
| `comparison_summary.csv` | Gauss-Newton vs Newton summary (18 rows) |

Additional CSV files are produced by individual test modules during a full CLI run.

## GUI Usage

1. Select a model (`exponential`, `logistic`, `sinusoidal`)
2. Select a method (`gauss_newton`, `newton`)
3. Set noise sigma on the slider
4. Set `theta0` values
5. Click **Run**
6. Click **Export log** to write the log text area to `fit_log.txt`

Each **Run** updates the fit plot, the convergence plot, the status line, and the log. It also writes `fit_plot.xml`, `convergence_plot.xml`, and `run_export.txt`.

## GUI Plots

- **Left plot** — data points and fitted curve (`t` vs `y`)
- **Right plot** — residual norm per iteration

Each plot uses the natPlot toolbar included in `gui::plot::View`.

## Report

See `report_outline.txt` for the report section list.

## Contact

**Almir Mustafic** — [GitHub](https://github.com/AlmirMBH)

## Acknowledgments

natID libraries are by Prof. dr Izudin Dzafic (Academic Free License, version 3).
