#!/usr/bin/python3
import argparse
import csv
import subprocess


class Peer:
    def __init__(self, host, port, priv_key, pub_key):
        self.host = host
        self.port = port
        self.priv_key = priv_key
        self.pub_key = pub_key


# returns a list of [public_key, private_key]
def generate_keypair():
    gen_keys = subprocess.check_output(["./ed25519-cli", "keygen"]).decode('UTF-8').split('\n')
    keys = []
    keys.append(gen_keys[0].split(':')[1].replace(' ', ''))
    keys.append(gen_keys[1].split(':')[1].replace(' ', ''))
    return keys

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("file",
                        help="pass file path to the peers.csv file")
    args = parser.parse_args()

    peers = []
    with open(args.file) as peers_file:
        peersreader = csv.reader(peers_file, delimiter=';')
        for peer in peersreader:
            keys = generate_keypair()
            peers.append(";".join((peer[0], peer[1], keys[0], keys[1])) + '\n')

    with open(args.file, 'w') as peers_file:
        peers_file.write("host;port;priv_key_b64_encoded;pub_key_b64_encoded\n")
        peers_file.writelines(peers)


if __name__ == "__main__":
    main()
