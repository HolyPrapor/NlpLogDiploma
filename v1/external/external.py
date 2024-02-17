import subprocess
import os


def encode_bzip(input_file, output_file, args=[]):
    out_file = open(output_file, mode="wb")
    process = subprocess.Popen(["bzip2", "-c", input_file] + args, stdout=out_file)
    process.wait()
    out_file.close()


def encode_rar(input_file, output_file, args=[]):
    process = subprocess.Popen(
        ["rar", "a", output_file, input_file] + args, stdout=open(os.devnull, "w")
    )
    process.wait()


def encode_7z(input_file, output_file, args=[]):
    process = subprocess.Popen(
        ["7z", "a", output_file, input_file] + args, stdout=open(os.devnull, "w")
    )
    process.wait()


def encode_gzip(input_file, output_file):
    out_file = open(output_file, mode="wb")
    process = subprocess.Popen(["gzip", "-c", input_file], stdout=out_file)
    process.wait()
    out_file.close()
