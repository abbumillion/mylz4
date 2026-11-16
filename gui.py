import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import os
import time

class MyLZ4GUI:
    def __init__(self, root):
        self.root = root
        self.root.title("mylz4 Compression Engine")
        self.root.geometry("750x550")
        self.root.configure(padx=20, pady=20, bg="#1e1e1e")

        # Header
        title = tk.Label(root, text="mylz4 Compression Engine", font=("Helvetica", 20, "bold"), fg="#ffffff", bg="#1e1e1e")
        title.pack(pady=10)

        # File Selection
        self.file_path = tk.StringVar()
        self.file_label = tk.Label(root, text="No file selected", textvariable=self.file_path, fg="#a0a0a0", bg="#1e1e1e", wraplength=700)
        self.file_label.pack(pady=5)

        # Buttons
        btn_frame = tk.Frame(root, bg="#1e1e1e")
        btn_frame.pack(pady=15)

        tk.Button(btn_frame, text="Browse File", command=self.browse_file, width=15, bg="#3c3f41", fg="white", font=("Helvetica", 10, "bold")).pack(side=tk.LEFT, padx=10)
        
        self.comp_btn = tk.Button(btn_frame, text="Compress", command=self.compress_file, width=15, bg="#4CAF50", fg="white", font=("Helvetica", 10, "bold"), state=tk.DISABLED)
        self.comp_btn.pack(side=tk.LEFT, padx=10)

        self.decomp_btn = tk.Button(btn_frame, text="Decompress", command=self.decompress_file, width=15, bg="#2196F3", fg="white", font=("Helvetica", 10, "bold"), state=tk.DISABLED)
        self.decomp_btn.pack(side=tk.LEFT, padx=10)

        # Stats Frame
        self.stats_frame = tk.Frame(root, bg="#2d2d2d", bd=2, relief=tk.GROOVE)
        self.stats_frame.pack(fill=tk.X, pady=10, padx=10)

        self.lbl_in_size = tk.Label(self.stats_frame, text="Input Size: -", fg="#d4d4d4", bg="#2d2d2d", font=("Courier", 10))
        self.lbl_in_size.pack(anchor="w", padx=10, pady=2)
        
        self.lbl_out_size = tk.Label(self.stats_frame, text="Output Size: -", fg="#d4d4d4", bg="#2d2d2d", font=("Courier", 10))
        self.lbl_out_size.pack(anchor="w", padx=10, pady=2)
        
        self.lbl_ratio = tk.Label(self.stats_frame, text="Ratio: -", fg="#d4d4d4", bg="#2d2d2d", font=("Courier", 10))
        self.lbl_ratio.pack(anchor="w", padx=10, pady=2)
        
        self.lbl_speed = tk.Label(self.stats_frame, text="Speed: -", fg="#4CAF50", bg="#2d2d2d", font=("Courier", 10, "bold"))
        self.lbl_speed.pack(anchor="w", padx=10, pady=2)
        
        self.lbl_time = tk.Label(self.stats_frame, text="Time: -", fg="#d4d4d4", bg="#2d2d2d", font=("Courier", 10))
        self.lbl_time.pack(anchor="w", padx=10, pady=2)

        # Log Output
        log_frame = tk.Frame(root, bg="#1e1e1e")
        log_frame.pack(fill=tk.BOTH, expand=True, pady=10)
        
        self.log = tk.Text(log_frame, height=5, bg="#111111", fg="#00ff00", font=("Courier", 10), bd=0, padx=10, pady=10)
        self.log.pack(fill=tk.BOTH, expand=True)
        self.log_msg("Ready. Select a file to begin testing.\n")

    def log_msg(self, msg):
        self.log.insert(tk.END, msg + "\n")
        self.log.see(tk.END)

    def format_bytes(self, size):
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size < 1024.0:
                return f"{size:.2f} {unit}"
            size /= 1024.0
        return f"{size:.2f} TB"

    def browse_file(self):
        filename = filedialog.askopenfilename()
        if filename:
            self.file_path.set(filename)
            self.comp_btn.config(state=tk.NORMAL)
            self.decomp_btn.config(state=tk.NORMAL)
            self.log_msg(f"Selected: {os.path.basename(filename)}")

    def run_tool(self, mode, in_path, out_path):
        exe = "./build/mylz4_cli"
        
        # Use perf_counter for high precision timing
        start = time.perf_counter()
        try:
            result = subprocess.run([exe, mode, in_path, out_path], capture_output=True, text=True)
            elapsed = time.perf_counter() - start
            
            if result.returncode == 0:
                in_size = os.path.getsize(in_path)
                out_size = os.path.getsize(out_path)
                ratio = (in_size / out_size) if out_size > 0 else 0
                
                # Calculate MB/s (using the original uncompressed size)
                speed_mbs = (in_size / 1_000_000) / elapsed if elapsed > 0 else 0
                
                # Update GUI Labels
                self.lbl_in_size.config(text=f"Input Size:  {self.format_bytes(in_size)}")
                self.lbl_out_size.config(text=f"Output Size: {self.format_bytes(out_size)}")
                self.lbl_ratio.config(text=f"Ratio:       {ratio:.2f}x")
                self.lbl_speed.config(text=f"Speed:       {speed_mbs:.1f} MB/s")
                self.lbl_time.config(text=f"Time:        {elapsed*1000:.2f} ms")
                
                self.log_msg(f"SUCCESS: {result.stdout.strip()}")
            else:
                self.log_msg(f"FAILED:\n{result.stderr}")
        except Exception as e:
            self.log_msg(f"Error: {e}")

    def compress_file(self):
        in_path = self.file_path.get()
        if not in_path: return
        out_path = in_path + ".mylz4"
        self.log_msg(f"\n--- Compressing {os.path.basename(in_path)} ---")
        self.run_tool("-c", in_path, out_path)

    def decompress_file(self):
        in_path = self.file_path.get()
        if not in_path: return
        if not in_path.endswith(".mylz4"):
            self.log_msg("Error: Please select a .mylz4 file to decompress.")
            return
        out_path = in_path.replace(".mylz4", ".out")
        self.log_msg(f"\n--- Decompressing {os.path.basename(in_path)} ---")
        self.run_tool("-d", in_path, out_path)

if __name__ == "__main__":
    root = tk.Tk()
    app = MyLZ4GUI(root)
    root.mainloop()
