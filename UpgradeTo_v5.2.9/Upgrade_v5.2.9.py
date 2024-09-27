import sys
import os
import logging
import subprocess
import venv
import platform
import ctypes

def setup_logger():
    """
    Set up and configure the logger for the application.
    Returns a logger object with a StreamHandler and custom formatter.
    """
    logger = logging.getLogger('DSMRlogger32_Flasher')
    logger.setLevel(logging.DEBUG)
    
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    
    formatter = logging.Formatter('%(name)s - %(levelname)s - %(message)s')
    ch.setFormatter(formatter)
    
    logger.addHandler(ch)
    
    return logger

def enable_long_paths():
    """
    Enable long path support on Windows if available.
    This allows handling of file paths longer than 260 characters.
    """
    if platform.system() == 'Windows':
        try:
            long_path_flag = 0x8000
            ctypes.windll.kernel32.SetProcessShortNameFormatW(long_path_flag)
        except AttributeError:
            # The function is not available, likely an older Windows version
            pass

def create_venv(venv_path):
    """
    Create a virtual environment at the specified path.
    Handles potential permission errors and exits if creation fails.
    
    :param venv_path: Path where the virtual environment should be created
    """
    try:
        venv.create(venv_path, with_pip=True)
    except PermissionError:
        logger.error(f"Permission denied when creating virtual environment at {venv_path}. Try running the script with administrator privileges.")
        sys.exit(1)

def install_requirements(venv_path):
    """
    Install required packages (esptool and pyserial) in the virtual environment.
    
    :param venv_path: Path to the virtual environment
    """
    if platform.system() == 'Windows':
        pip_path = os.path.join(venv_path, 'Scripts', 'pip.exe')
    else:
        pip_path = os.path.join(venv_path, 'bin', 'pip')
    try:
        subprocess.check_call([pip_path, 'install', 'esptool', 'pyserial'])
    except subprocess.CalledProcessError as e:
        logger.error(f"Error installing requirements: {e}")
        sys.exit(1)

def flash_esp32(port, logger, venv_path):
    """
    Flash the ESP32 device with the DSMRlogger32 firmware.
    
    :param port: Serial port to use for flashing
    :param logger: Logger object for output
    :param venv_path: Path to the virtual environment
    """
    logger.info(f"Working directory: {os.getcwd()}")

    required_files = [
        'bootloader.bin',
        'firmware_v5.2.9.bin',
        'partitions.bin',
        'spiffs_v5.2.9.bin'
    ]
    for file in required_files:
        file_path = os.path.abspath(file)
        if not os.path.exists(file_path):
            logger.error(f"Error: {file} not found in {os.getcwd()}")
            return

    if platform.system() == 'Windows':
        python_path = os.path.join(venv_path, 'Scripts', 'python.exe')
    else:
        python_path = os.path.join(venv_path, 'bin', 'python')

    cmd_args = [
        python_path,
        '-m', 'esptool',
        '--chip', 'esp32',
        '--port', port,
        '--baud', '230400',
        '--before', 'default_reset',
        '--after', 'hard_reset',
        'write_flash', '-z',
        '--flash_mode', 'dio',
        '--flash_freq', '80m',
        '--flash_size', '4MB',
        '0x1000', os.path.abspath('bootloader.bin'),
        '0x8000', os.path.abspath('partitions.bin'),
        '0x10000', os.path.abspath('firmware_v5.2.9.bin'),
        '0x310000', os.path.abspath('spiffs_v5.2.9.bin')
    ]

    logger.info(f"Executing esptool with arguments: {' '.join(cmd_args)}")

    try:
        subprocess.check_call(cmd_args)
        logger.info("DSMRlogger32 flashed successfully!")
    except subprocess.CalledProcessError as e:
        logger.error(f"Error flashing DSMRlogger32: {e}")

if __name__ == "__main__":
    # Set up logging
    logger = setup_logger()
    
    # Check for correct usage
    if len(sys.argv) != 2:
        logger.error("Usage: python Upgrade_v5.2.9.py <serial_port>")
        sys.exit(1)
    
    # Enable long path support on Windows
    enable_long_paths()
    
    # Get the serial port from command line argument
    serial_port = sys.argv[1]
    logger.info(f"Starting DSMRlogger32 flashing process on port: {serial_port}")

    # Set up virtual environment
    venv_path = os.path.abspath(os.path.join(os.getcwd(), 'venv'))
    if not os.path.exists(venv_path):
        logger.info("Creating virtual environment...")
        create_venv(venv_path)
        logger.info("Installing required packages...")
        install_requirements(venv_path)
    
    # Flash the ESP32
    flash_esp32(serial_port, logger, venv_path)
    logger.info("DSMRlogger32 flashing process completed.")