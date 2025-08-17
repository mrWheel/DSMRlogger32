#!/usr/bin/env python3
"""
DSMR Logger File Downloader (with progress bar)
Downloads a specific file from DSMR-logger32 filesystem via HTTP API

Usage:
  python3 dsmrDownloader.py                           # Interactive mode
  python3 dsmrDownloader.py <host>                    # Specify host, ask for filename
  python3 dsmrDownloader.py <host> <filename>         # Specify both host and filename
  
Examples:
  python3 dsmrDownloader.py 192.168.12.160
  python3 dsmrDownloader.py DSMR-PRD32.local settings.ini
"""

import urllib.request
import urllib.parse
import urllib.error
import os
import sys
from datetime import datetime

def show_progress(current, total, description="Progress"):
    """Simple progress display"""
    if total > 0:
        percent = (current / total) * 100
        bar_length = 40
        filled_length = int(bar_length * current // total)
        bar = '█' * filled_length + '-' * (bar_length - filled_length)
        
        current_mb = current / (1024 * 1024)
        total_mb = total / (1024 * 1024)
        
        print(f'\r{description}: |{bar}| {percent:.1f}% ({current_mb:.2f}/{total_mb:.2f} MB)', end='', flush=True)
    else:
        current_mb = current / (1024 * 1024)
        print(f'\r{description}: {current_mb:.2f} MB', end='', flush=True)

class DSMRFileDownloader:
    def __init__(self, host: str):
        """Initialize with host (IP address or mDNS name)"""
        if not host.startswith('http'):
            self.base_url = f"http://{host}"
        else:
            self.base_url = host.rstrip('/')

    def test_connection(self) -> bool:
        """Test if we can connect to the DSMR logger"""
        try:
            with urllib.request.urlopen(f"{self.base_url}/", timeout=10) as response:
                return response.status == 200
        except (urllib.error.URLError, urllib.error.HTTPError):
            return False

    def download_file(self, filename: str) -> bool:
        """Download a file from the DSMR logger with datetime prefix and progress bar"""
        try:
            # Ensure filename starts with /
            if not filename.startswith('/'):
                filename = '/' + filename
            
            # URL encode the filename
            encoded_filename = urllib.parse.quote(filename)
            download_url = f"{self.base_url}{encoded_filename}?download=1"
            
            print(f"Downloading {filename}...")
            
            # Create request
            req = urllib.request.Request(download_url)
            
            with urllib.request.urlopen(req, timeout=30) as response:
                if response.status == 200:
                    # Get file size from headers
                    content_length = response.headers.get('Content-Length')
                    total_size = int(content_length) if content_length else 0
                    
                    # Get just the filename without path
                    original_filename = os.path.basename(filename)
                    if not original_filename:
                        original_filename = "downloaded_file"
                    
                    # Create datetime prefix (YYYYMMDD-HHMM)
                    now = datetime.now()
                    datetime_prefix = now.strftime("%Y%m%d-%H%M")
                    
                    # Create new filename with datetime prefix
                    local_filename = f"{datetime_prefix}_{original_filename}"
                    
                    # Write file with progress tracking
                    downloaded = 0
                    with open(local_filename, 'wb') as f:
                        while True:
                            chunk = response.read(8192)
                            if not chunk:
                                break
                            f.write(chunk)
                            downloaded += len(chunk)
                            show_progress(downloaded, total_size, "Downloading")
                    
                    print()  # New line after progress
                    
                    file_size = os.path.getsize(local_filename)
                    print(f"✓ Successfully downloaded '{filename}' ({file_size} bytes)")
                    print(f"✓ Saved as: '{local_filename}'")
                    return True
                else:
                    print(f"✗ Error downloading file: HTTP {response.status}")
                    return False
                    
        except urllib.error.HTTPError as e:
            if e.code == 404:
                print(f"✗ File '{filename}' not found on device")
            else:
                print(f"✗ HTTP error downloading file: {e.code} {e.reason}")
            return False
        except urllib.error.URLError as e:
            print(f"✗ Network error downloading file: {e.reason}")
            return False
        except IOError as e:
            print(f"✗ Error saving file: {e}")
            return False


def parse_arguments() -> tuple[str, str]:
    """Parse command line arguments and return host and filename"""
    args = sys.argv[1:]  # Skip script name
    
    if len(args) == 0:
        # Interactive mode - ask for both
        return get_interactive_input()
    elif len(args) == 1:
        # Host provided, ask for filename
        host = args[0]
        filename = get_filename_input()
        return host, filename
    elif len(args) == 2:
        # Both host and filename provided
        host = args[0]
        filename = args[1]
        return host, filename
    else:
        # Too many arguments
        print_usage()
        sys.exit(1)


def get_interactive_input() -> tuple[str, str]:
    """Get host and filename interactively"""
    print("DSMR Logger File Downloader")
    print("=" * 40)
    
    # Get host (IP or mDNS name)
    while True:
        host = input("Enter DSMR logger IP address or mDNS name (e.g., 192.168.1.100 or dsmr-logger.local): ").strip()
        if host:
            break
        print("Please enter a valid IP address or hostname.")
    
    # Get filename
    filename = get_filename_input()
    
    return host, filename


def get_filename_input() -> str:
    """Get filename from user input"""
    while True:
        filename = input("Enter filename to download (e.g., settings.ini, RINGhours.csv, data/config.json): ").strip()
        if filename:
            break
        print("Please enter a filename.")
    
    return filename


def print_usage():
    """Print usage information"""
    script_name = os.path.basename(sys.argv[0])
    print(f"""
DSMR Logger File Downloader

Usage:
  {script_name}                           # Interactive mode
  {script_name} <host>                    # Specify host, ask for filename
  {script_name} <host> <filename>         # Specify both host and filename
  
Examples:
  {script_name} 192.168.12.160
  {script_name} DSMR-PRD32.local settings.ini
  {script_name} 192.168.1.100 data/config.json

Notes:
  - Downloaded files are saved with datetime prefix (YYYYMMDD-HHMM_filename)
  - Files are saved to the current directory
  - Progress bar shows download status
""")


def main():
    """Main function"""
    try:
        # Handle help request
        if len(sys.argv) > 1 and sys.argv[1] in ['-h', '--help', 'help']:
            print_usage()
            return 0
        
        # Parse arguments
        host, filename = parse_arguments()
        
        print(f"Host: {host}")
        print(f"File: {filename}")
        
        # Create downloader instance
        downloader = DSMRFileDownloader(host)
        
        # Test connection
        print(f"\nTesting connection to {host}...")
        if not downloader.test_connection():
            print(f"✗ Cannot connect to {host}. Please check:")
            print("  - IP address or hostname is correct")
            print("  - Device is powered on and connected to network")
            print("  - No firewall blocking the connection")
            return 1
        
        print(f"✓ Connected to {host}")
        
        # Download the file
        success = downloader.download_file(filename)
        
        return 0 if success else 1
        
    except KeyboardInterrupt:
        print("\n\nDownload cancelled by user.")
        return 1
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
