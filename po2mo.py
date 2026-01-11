import struct
import sys
import os

def generate_mo(po_file, mo_file):
    messages = {}
    current_msgid = None
    current_msgstr = None

    with open(po_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    for line in lines:
        line = line.strip()
        if line.startswith('msgid "'):
            current_msgid = line[7:-1]
        elif line.startswith('msgstr "'):
            current_msgstr = line[8:-1]
            if current_msgid is not None:
                # Handle escaped quotes and newlines if necessary (basic handling here)
                current_msgid = current_msgid.replace('\\"', '"').replace('\\n', '\n')
                current_msgstr = current_msgstr.replace('\\"', '"').replace('\\n', '\n')
                messages[current_msgid] = current_msgstr
                current_msgid = None

    # Sort keys
    keys = sorted(messages.keys())
    
    # Header
    magic = 0x950412de
    revision = 0
    num_strings = len(keys)
    
    # Calculate offsets
    # Header is 7 * 4 bytes = 28 bytes
    orig_table_offset = 28
    trans_table_offset = orig_table_offset + num_strings * 8
    
    # Data starts after tables
    data_offset = trans_table_offset + num_strings * 8
    
    # Prepare buffers
    orig_table = []
    trans_table = []
    data = bytearray()
    
    current_data_offset = data_offset
    
    # Process original strings
    for key in keys:
        encoded_key = key.encode('utf-8') + b'\0'
        orig_table.append((len(encoded_key) - 1, current_data_offset))
        data.extend(encoded_key)
        current_data_offset += len(encoded_key)
        
    # Process translated strings
    for key in keys:
        val = messages[key]
        encoded_val = val.encode('utf-8') + b'\0'
        trans_table.append((len(encoded_val) - 1, current_data_offset))
        data.extend(encoded_val)
        current_data_offset += len(encoded_val)

    # Write file
    with open(mo_file, 'wb') as f:
        # Header
        f.write(struct.pack('Iiiiiii', magic, revision, num_strings, orig_table_offset, trans_table_offset, 0, 0))
        
        # Original string table
        for length, offset in orig_table:
            f.write(struct.pack('ii', length, offset))
            
        # Translated string table
        for length, offset in trans_table:
            f.write(struct.pack('ii', length, offset))
            
        # Data
        f.write(data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 po2mo.py <input.po> <output.mo>")
        sys.exit(1)
        
    po_path = sys.argv[1]
    mo_path = sys.argv[2]
    
    # Ensure output directory exists
    os.makedirs(os.path.dirname(mo_path), exist_ok=True)
    
    generate_mo(po_path, mo_path)
    print(f"Compiled {po_path} to {mo_path}")
