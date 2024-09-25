import sys
import os
import logging
import subprocess
import venv

def setup_logger():
    logger = logging.getLogger('DSMRlogger32_Flasher')
    logger.setLevel(logging.DEBUG)
    
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    ch.setFormatter(formatter)
    
    logger.addHandler(ch)
    
    return logger

def create_venv(venv_path):
    venv.create(venv_path, with_pip=True)

def install_requirements(venv_path):
    pip_path = os.path.join(venv_path, 'bin', 'pip')
    subprocess.check_call([pip_path, 'install', 'esptool', 'pyserial'])

def flash_esp32(port, logger, venv_path):
    logger.info(f"Working directory: {os.getcwd()}")

    required_files = [
        'bootloader.bin',
        'firmware_v5.2.9.bin',
        'partitions.bin',
        'spiffs_v5.2.9.bin'
    ]
    for file in required_files:
        if not os.path.exists(file):
            logger.error(f"Error: {file} not found in {os.getcwd()}")
            return

    cmd_args = [
        os.path.join(venv_path, 'bin', 'python'),
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
        '0x1000', 'bootloader.bin',
        '0x8000', 'partitions.bin',
        '0x10000', 'firmware_v5.2.9.bin',
        '0x310000', 'spiffs_v5.2.9.bin'
    ]

    logger.info(f"Executing esptool with arguments: {' '.join(cmd_args)}")

    try:
        subprocess.check_call(cmd_args)
        logger.info("DSMRlogger32 flashed successfully!")
    except subprocess.CalledProcessError as e:
        logger.error(f"Error flashing DSMRlogger32: {e}")

if __name__ == "__main__":
    logger = setup_logger()
    
    if len(sys.argv) != 2:
        logger.error("Usage: python Upgrade_v5.2.9.py <serial_port>")
        sys.exit(1)
    
    serial_port = sys.argv[1]
    logger.info(f"Starting DSMRlogger32 flashing process on port: {serial_port}")

    venv_path = os.path.join(os.getcwd(), 'venv')
    if not os.path.exists(venv_path):
        logger.info("Creating virtual environment...")
        create_venv(venv_path)
        logger.info("Installing required packages...")
        install_requirements(venv_path)
    
    flash_esp32(serial_port, logger, venv_path)
    logger.info("DSMRlogger32 flashing process completed.")