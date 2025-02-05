import struct

def read_wav_file(filename):
    with open(filename, "rb") as f:
        # Read WAV header
        header = f.read(44)  # Standard WAV header is 44 bytes
        
        # Extract important parameters
        num_channels = struct.unpack("<H", header[22:24])[0]
        sample_rate = struct.unpack("<I", header[24:28])[0]
        bits_per_sample = struct.unpack("<H", header[34:36])[0]
        
        print(f"Channels: {num_channels}, Sample Rate: {sample_rate} Hz, Bits Per Sample: {bits_per_sample}")

        # Read audio data
        audio_data = f.read()

    # Convert to 16-bit range (0-65535)
    if bits_per_sample == 8:
        # 8-bit PCM (unsigned) → Convert to 16-bit range
        samples = [sample * 256 for sample in audio_data]  # Scale 8-bit (0-255) to 16-bit (0-65535)
    
    elif bits_per_sample == 16:
        # 16-bit PCM (signed) → Convert to 16-bit unsigned range (0-65535)
        samples = [struct.unpack("<h", audio_data[i:i+2])[0] + 32768 for i in range(0, len(audio_data), 2)]
    
    else:
        raise ValueError("Unsupported WAV format. Use 8-bit or 16-bit PCM.")

    # Convert to bytearray
    pwm_data = bytearray(struct.pack("<H", sample) for sample in samples)

    return pwm_data, sample_rate

# Example usage
pwm_bytearray, sample_rate = read_wav_file("audio.wav")

# Print first few values
print(pwm_bytearray[:20])  # Show first 10 samples (each is 2 bytes)
