#!/usr/bin/env python3
"""
DSMR Logger File Uploader (with datetime prefix removal)
Uploads files to DSMR-logger32 filesystem via HTTP API

If uploading a file with YYYYMMDD-HHMM_ prefix, the prefix is removed on SPIFFS.
Example: "20250817-1034_setup.ini" uploads as "setup.ini" on SPIFFS

Usage:
  python3 dsmrUploader.py                           # Interactive mode
  python3 dsmrUploader.py <host>                    # Specify host, ask for filename
  python3 dsmrUploader.py <host> <filename>         # Specify both host and filename
  
Examples:
  python3 dsmrUploader.py 192.168.12.160
  python3 dsmrUploader.py DSMR-PRD32.local 20250817-1034_settings.ini
"""

import urllib.request
import urllib.parse
import urllib.error
import os
import sys
import mimetypes
import re
from datetime import datetime

class DSMRFileUploader:
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

    def has_datetime_prefix(self, filename: str) -> bool:
        """Check if filename has YYYYMMDD-HHMM_ prefix"""
        pattern = r'^\d{8}-\d{4}_(.+)$'
        return bool(re.match(pattern, filename))

    def remove_datetime_prefix(self, filename: str) -> str:
        """Remove YYYYMMDD-HHMM_ prefix from filename if present"""
        pattern = r'^\d{8}-\d{4}_(.+)$'
        match = re.match(pattern, filename)
        if match:
            return match.group(1)
        return filename

    def upload_file(self, local_filepath: str) -> bool:
        """Upload a file to the DSMR logger"""
        try:
            # Check if local file exists
            if not os.path.exists(local_filepath):
                print(f"✗ Local file '{local_filepath}' not found")
                return False
            
            # Get file info
            local_filename = os.path.basename(local_filepath)
            file_size = os.path.getsize(local_filepath)
            
            # Determine target filename (remove prefix if present)
            target_filename = self.remove_datetime_prefix(local_filename)
            
            # Show what we're doing
            if local_filename != target_filename:
                print(f"Uploading '{local_filepath}' ({file_size} bytes)")
                print(f"Target filename on SPIFFS: '{target_filename}' (prefix removed)")
            else:
                print(f"Uploading '{local_filepath}' ({file_size} bytes) as '{target_filename}'")
            
            # Read file content
            with open(local_filepath, 'rb') as f:
                file_content = f.read()
            
            # Create multipart form data
            boundary = f"----WebKitFormBoundary{datetime.now().strftime('%Y%m%d%H%M%S')}"
            
            # Build multipart body
            body_parts = []
            
            # Add file field (use target_filename for upload)
            body_parts.append(f'--{boundary}'.encode())
            body_parts.append(f'Content-Disposition: form-data; name="upload"; filename="{target_filename}"'.encode())
            
            # Guess content type
            content_type, _ = mimetypes.guess_type(target_filename)
            if content_type is None:
                content_type = 'application/octet-stream'
            body_parts.append(f'Content-Type: {content_type}'.encode())
            body_parts.append(b'')
            body_parts.append(file_content)
            
            # End boundary
            body_parts.append(f'--{boundary}--'.encode())
            
            # Join with CRLF
            body = b'\r\n'.join(body_parts)
            
            # Create request
            upload_url = f"{self.base_url}/upload"
            req = urllib.request.Request(upload_url, data=body)
            req.add_header('Content-Type', f'multipart/form-data; boundary={boundary}')
            req.add_header('Content-Length', str(len(body)))
            
            # Send request
            try:
                with urllib.request.urlopen(req, timeout=120) as response:
                    if response.status in [200, 303]:  # 303 is redirect after successful upload
                        print(f"✓ Successfully uploaded as '{target_filename}' to DSMR logger")
                        return True
                    else:
                        print(f"✗ Error uploading file: HTTP {response.status}")
                        response_text = response.read().decode('utf-8', errors='ignore')
                        if response_text:
                            print(f"Response: {response_text[:200]}...")
                        return False
                        
            except urllib.error.HTTPError as e:
                # Check if it's actually successful (ESP32 sometimes returns 303 as error)
                if e.code == 303:
                    print(f"✓ Successfully uploaded as '{target_filename}' to DSMR logger")
                    return True
                else:
                    print(f"✗ HTTP error uploading file: {e.code} {e.reason}")
                    try:
                        error_response = e.read().decode('utf-8', errors='ignore')
                        if error_response:
                            print(f"Error details: {error_response[:200]}...")
                    except:
                        pass
                    return False
                    
        except urllib.error.URLError as e:
            print(f"✗ Network error uploading file: {e.reason}")
            return False
        except IOError as e:
            print(f"✗ Error reading local file: {e}")
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
    print("DSMR Logger File Uploader")
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
        filename = input("Enter local file path to upload (e.g., ./settings.ini, 20250817-1034_config.json): ").strip()
        if filename:
            # Expand user path (~)
            filename = os.path.expanduser(filename)
            if os.path.exists(filename):
                break
            else:
                print(f"File '{filename}' not found. Please enter a valid file path.")
        else:
            print("Please enter a filename.")
    
    return filename


def print_usage():
    """Print usage information"""
    script_name = os.path.basename(sys.argv[0])
    print(f"""
DSMR Logger File Uploader

Usage:
  {script_name}                           # Interactive mode
  {script_name} <host>                    # Specify host, ask for filename
  {script_name} <host> <filename>         # Specify both host and filename
  
Examples:
  {script_name} 192.168.12.160
  {script_name} DSMR-PRD32.local ./settings.ini
  {script_name} 192.168.1.100 20250817-1034_config.json

Notes:
  - Files with YYYYMMDD-HHMM_ prefix will have the prefix removed on SPIFFS
  - Files without the prefix are uploaded as-is
  - Example: "20250817-1034_setup.ini" becomes "setup.ini" on SPIFFS
""")


def main():
    """Main function"""
    try:
        # Handle help request
        if len(sys.argv) > 1 and sys.argv[1] in ['-h', '--help', 'help']:
            print_usage()
            return 0
        
        # Parse arguments
        host, local_filepath = parse_arguments()
        
        print(f"Host: {host}")
        print(f"Local file: {local_filepath}")
        
        # Create uploader instance
        uploader = DSMRFileUploader(host)
        
        # Test connection
        print(f"\nTesting connection to {host}...")
        if not uploader.test_connection():
            print(f"✗ Cannot connect to {host}. Please check:")
            print("  - IP address or hostname is correct")
            print("  - Device is powered on and connected to network")
            print("  - No firewall blocking the connection")
            return 1
        
        print(f"✓ Connected to {host}")
        
        # Upload the file
        success = uploader.upload_file(local_filepath)
        
        return 0 if success else 1
        
    except KeyboardInterrupt:
        print("\n\nUpload cancelled by user.")
        return 1
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
