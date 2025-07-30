# CSV Simulator - Real Time Project 2

A comprehensive CSV file processing and simulation system implemented in C with GUI support, file operations, and multi-threaded processing capabilities.

## About

This is a real-time project that simulates CSV file operations with advanced features including GUI interface, file generation, data processing, and inspector tools. The project demonstrates complex system programming concepts including file I/O, memory management, multi-threading, and user interface development.

## Project Overview

The CSV Simulator is designed to handle large-scale CSV data processing with real-time capabilities. It features modular architecture with separate components for file operations, data processing, visualization, and system inspection.

## Key Features

### Core Functionality
- **CSV File Processing** - Read, parse, and manipulate CSV data efficiently
- **Real-Time Simulation** - Live data processing and updates
- **File Generation** - Create CSV files with custom data patterns
- **Data Validation** - Inspect and validate CSV file integrity

### Advanced Features
- **GUI Interface** - User-friendly graphical interface for interaction
- **Multi-Threading** - Concurrent processing for better performance
- **Memory Management** - Efficient shared memory operations
- **Message Queues** - Inter-process communication system
- **Multiple Inspectors** - Different types of data inspection tools

### System Components
- **File Operations** - Generate, move, and manipulate files
- **Data Processing** - CSV calculation and analysis engine
- **Drawing Module** - Visualization and graphics rendering
- **Inspector Types** - Multiple inspection and validation tools

## Project Structure

```
CSV_Simulator/
├── .idea/                     # IDE configuration
├── .gitignore                # Git ignore rules
├── Dockerfile                # Container configuration
├── README.md                 # Project documentation
├── arguments.txt             # Runtime arguments
├── constants.h               # Project constants and definitions
├── docker-compose.yml        # Docker orchestration
├── makefile                  # Build automation
├── run.sh                    # Execution script

# Core Modules
├── csv_calculator           # CSV calculation engine
├── csv_calculator.c         # Implementation
├── drawer                   # Graphics and visualization
├── drawer.c                 # Drawing implementation
├── file_generator           # File creation utilities
├── file_generator.c         # File generation logic
├── file_mover              # File management system
├── file_mover.c            # File operations implementation

# System Components
├── functions.h              # Function declarations
├── header.h                # Main header file
├── main                    # Main executable
├── main.c                  # Application entry point
├── message_queues.h        # Message queue definitions
├── semphores.h             # Semaphore definitions
├── shared_memories.h       # Shared memory structures

# Inspector Tools
├── inspector_type1         # Primary inspector
├── inspector_type1.c       # Type 1 inspection logic
├── inspector_type2.c       # Type 2 inspection implementation
├── inspector_type3.c       # Type 3 inspection features

# Memory Management
├── read_shared_memory      # Memory reader utility
├── read_shared_memory.c    # Shared memory operations
```

## Technologies Used

- **Programming Language**: C
- **Build System**: Make
- **Containerization**: Docker & Docker Compose
- **System Programming**: POSIX threads, shared memory, semaphores
- **File Processing**: CSV parsing and manipulation
- **GUI Development**: [Graphics library used]
- **Version Control**: Git

## Installation & Setup

### Prerequisites
- GCC compiler (4.9+)
- Make utility
- Docker (optional)
- POSIX-compatible system (Linux/Unix)
- Required system libraries for GUI components

### Local Development

1. **Clone the repository:**
   ```bash
   git clone https://github.com/faris771/CSV_Simulator.git
   cd CSV_Simulator
   ```

2. **Build the project:**
   ```bash
   make clean
   make
   ```

3. **Run the simulator:**
   ```bash
   ./run.sh
   ```

### Docker Deployment

1. **Using Docker Compose:**
   ```bash
   docker-compose up --build
   ```

2. **Manual Docker build:**
   ```bash
   docker build -t csv-simulator .
   docker run -it csv-simulator
   ```

## Usage

### Basic Operation

1. **Configure arguments:**
   ```bash
   # Edit arguments.txt with your parameters
   nano arguments.txt
   ```

2. **Start the simulator:**
   ```bash
   ./main
   ```

3. **Use inspector tools:**
   ```bash
   ./inspector_type1
   ./inspector_type2
   ./inspector_type3
   ```

### Advanced Features

#### CSV File Generation
```bash
# Generate test CSV files
./file_generator [parameters]
```

#### Real-Time Processing
```bash
# Start with CSV calculator
./csv_calculator input.csv
```

#### File Operations
```bash
# Move and organize files
./file_mover source destination
```

#### Data Visualization
```bash
# Display data graphics
./drawer data.csv
```

## System Architecture

### Multi-Process Design
- **Main Process**: Coordinates all operations
- **File Generator**: Creates CSV files with specified patterns
- **CSV Calculator**: Processes numerical data
- **Inspector Processes**: Validate and analyze data
- **Drawer Process**: Handles visualization

### Inter-Process Communication
- **Shared Memory**: High-speed data sharing between processes
- **Message Queues**: Reliable message passing
- **Semaphores**: Process synchronization and resource management

### Data Flow
1. File generation creates CSV data
2. Main process coordinates operations
3. Calculator processes numerical data
4. Inspectors validate results
5. Drawer visualizes output
6. File mover organizes results

## Key Components

### CSV Calculator (`csv_calculator.c`)
- Numerical analysis of CSV data
- Statistical calculations
- Data aggregation and reporting

### File Generator (`file_generator.c`)
- Creates CSV files with custom patterns
- Configurable data generation
- Performance optimization for large files

### Inspector Types
- **Type 1**: Basic data validation
- **Type 2**: Advanced pattern analysis  
- **Type 3**: Comprehensive data integrity checks

### Drawing Module (`drawer.c`)
- Data visualization and graphics
- Chart generation from CSV data
- Real-time display updates

### Shared Memory Management
- Efficient data sharing between processes
- Memory allocation and cleanup
- Thread-safe operations

## Configuration

### Build Configuration
```makefile
# Customize makefile for your environment
CC = gcc
CFLAGS = -Wall -pthread -std=c99
LIBS = -lm -lpthread
```

### Runtime Arguments
```bash
# arguments.txt format
CSV_FILE_PATH=/path/to/data.csv
NUM_PROCESSES=4
BUFFER_SIZE=1024
INSPECTION_LEVEL=2
```

## Performance Features

### Optimization Techniques
- **Multi-threading**: Parallel processing for large datasets
- **Memory Pooling**: Efficient memory allocation
- **Buffered I/O**: Optimized file operations
- **Process Synchronization**: Coordinated resource access

### Scalability
- Handles large CSV files (GB+ sizes)
- Configurable process count
- Dynamic memory allocation
- Real-time performance monitoring

## Testing & Validation

### Quality Assurance
- Multiple inspector types for data validation
- Memory leak detection
- Performance benchmarking
- Error handling and recovery

### Test Cases
- Large file processing
- Concurrent access scenarios
- Error condition handling
- Resource limit testing

## Development Highlights

### Advanced C Programming
- Complex data structures
- Multi-process architecture
- System-level programming
- Memory management expertise

### Real-Time Systems
- Process synchronization
- Inter-process communication
- Real-time data processing
- Performance optimization

## Troubleshooting

### Common Issues
- **Build Errors**: Check GCC version and dependencies
- **Permission Issues**: Ensure proper file permissions
- **Memory Errors**: Monitor shared memory usage
- **Process Sync**: Check semaphore implementations

### Debug Mode
```bash
# Compile with debug symbols
make debug

# Run with debugging
gdb ./main
```

## Contributing

This is an academic project, but suggestions are welcome:

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Test thoroughly
5. Submit a pull request

## Academic Context

This project demonstrates mastery of:
- Systems programming in C
- Multi-process architecture
- Real-time data processing
- File I/O operations
- Memory management
- Inter-process communication

## Future Enhancements

- [ ] Web-based GUI interface
- [ ] Database integration
- [ ] Network-based processing
- [ ] Machine learning integration
- [ ] Enhanced visualization tools

## Contact

**Faris Ahmad**  
GitHub: [@faris771](https://github.com/faris771)

**Collaborator: Lama Rimawi**  
GitHub: [@lamaRimawi](https://github.com/lamaRimawi)

This project showcases advanced C programming skills and real-time system development capabilities.

---

*Project Status: Complete | Type: Real-Time Systems | Language: C | Architecture: Multi-Process*
