# MNXB11 – Lund vs Umeå Climate Comparison

## About

This project is the final project for the MNXB11 course at Lund University.

The objective is to compare the climate of Lund and Umeå using historical meteorological data from the Swedish Meteorological and Hydrological Institute (SMHI).

The data is processed using Bash scripts and C++, while ROOT is used for statistical analysis and visualization.

## Research questions

We investigate four aspects of the climate:

1. **Seasonal temperature variation (Elena):** How does the mean temperature vary throughout the year in Lund and Umeå?
2. **Long-term temperature trends (Georges):** How has the mean annual temperature changed over time in both cities?
3. **Precipitation (Gabriel):** How does precipitation vary throughout the year, and which city receives more precipitation?
4. **Global solar irradiance (Manuel):** How does incoming solar radiation vary throughout the year, and how does the difference in latitude affect the measured irradiance?

## Project report

The project report, written in LaTeX, is available at
[Project-Report.pdf](Project-Report.pdf).

The report presents the methodology, statistical analysis, results,
and discussion of the climate comparison between Lund and Umeå.

## Repository structure

```text
MNXB11-Climate-Comparison/
├── datasets/
│   ├── raw_data/           # Original SMHI data
│   ├── derived_data/       # Cleaned CSV files
│   └── README.md           # Dataset documentation
├── results/
│   ├── sunshine/           # Generated solar irradiation plots
│   └── temperature/        # Generated daily mean temperature plots
│   └── precipitation/      # Generated daily mean precipitation plots
├── include/                # C++ header files
├── src/                    # C++ source files
├── scripts/
│   └── data/               # Data preprocessing scripts
├── slurm/                  # SLURM submission scripts
├── main.cxx                # Main program
├── Makefile                 # Compilation instructions
├── rootlogon.C             # ROOT configuration
├── Project-Report.pdf      # Final project report
├── Workplan.md             # Project tasks and timeline
└── ChangeLog               # Record of project changes
```

## Data

The project uses three meteorological parameters obtained from SMHI:

| Parameter | Unit |
|---|---|
| Temperature | °C |
| Precipitation | mm |
| Global solar irradiance | W/m² |

The original datasets are stored in `datasets/raw_data/`. Cleaned datasets are stored in `datasets/derived_data/`.

The data preparation scripts remove unnecessary metadata and format the measurements for C++ import.

Further information about the measurement stations, CSV formats, and preprocessing can be found in [datasets/README.md](datasets/README.md).

## Building and running

The project uses C++17.

### Requirements

- g++ (C++17 compatible)
- GNU Make
- [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser)
- ROOT (for statistical analysis and plotting)

### Installing the CSV parser

The project uses [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser).
Install the header-only library from the repository root:

```bash
mkdir -p external/include/csv
curl -fL https://raw.githubusercontent.com/ben-strasser/fast-cpp-csv-parser/master/csv.h \
    -o external/include/csv/csv.h
```

The Makefile already includes `-I external/include` in the compiler flags.

### Compiling and running

Ensure ROOT, g++ and GNU Make are available.

```bash
mkdir -p results/{sunshine,temperature,precipitation}
make
./main
```

To remove compiled files:

```bash
make clean
```

### Building with bash scripts

The project also includes scripts for building and running inside the MNXB11 course container on COSMOS.

Build the project:

```bash
bash scripts/buildproject.sh
```

This creates the required results directories and compiles the project using the course container.

Run the compiled program:

```bash
bash scripts/runproject.sh
```

The script executes `./main` inside the Apptainer container.

### Running with SLURM

Alternatively, the project can be executed as a batch job on COSMOS using the provided SLURM script.

First, build the executable as described above. Then submit the job from the repository root:

```bash
sbatch --chdir="$PWD" slurm/project.sbatch
```

Check the job status:

```bash
squeue -u "$USER"
```

Cancel a job if necessary:

```bash
scancel <job_id>
```

The batch script uses `scripts/runproject.sh` and currently requests 5 minutes of execution time and 1 GB of memory.

Job output is normally written to `slurm-<job_id>.out` in the submission directory.

## Data processing and analysis

Instructions for downloading and regenerating the processed datasets are available in [datasets/README.md](datasets/README.md).

The project follows these steps:

1. **Data preparation:** Bash scripts extract the relevant measurements from the original SMHI datasets.
2. **Data import and conversion:** C++ functions import the cleaned CSV files into memory. Dates are separated into year and day-of-year values (1–366), while hours are converted into integers (0–23).
3. **Statistical analysis:** Measurements are grouped and analysed to identify seasonal patterns, long-term changes, and differences between the cities.
4. **Visualization:** ROOT is used to produce plots for the climate comparisons.

The CSV importer is implemented in `src/parse_csv.cxx` and declared in `include/parse_csv.h`. It supports three-column and four-column cleaned datasets.

## Precipitation Analysis

All data is processed in `src/precipitation.cxx` using functions to extract data, calculate the relevant stats for each day, and plot it. The functions are used in `main.cxx` and the results are shown according to the repository structure. The type of plots generated are mean precipitation vs day number for a set of years. The header is in `include/precipitation.h`.

### Solar irradiance analysis

Solar irradiance data is processed in `src/sunshine.cxx`, using reusable functions to calculate daily and annual averages. Visualization is implemented separately in `src/sunshine_plot.cxx` using ROOT.

Three types of plots are generated:
- Average daily solar irradiation comparison between Lund and Umeå.
- Daily irradiation histograms showing the average and individual years.
- Annual mean solar irradiance comparison with linear regression fits to identify long-term trends.

The generated figures are saved in `results/sunshine/`.

## Team

- Elena Sanchez Nunez
- Gabriel Corrigan Fornvik
- Georges Nguyen
- Manuel Lorente Rodriguez

For individual responsibilities and deadlines, see [Workplan.md](Workplan.md).

## References

- [SMHI – Swedish Meteorological and Hydrological Institute](https://www.smhi.se/)
- [ROOT Data Analysis Framework](https://root.cern/)
- [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser)
