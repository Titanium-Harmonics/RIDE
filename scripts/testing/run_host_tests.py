#!/usr/bin/env python3
"""Run standalone host test suites and write a JUnit report."""
import argparse
import subprocess
import time
import xml.etree.ElementTree as ET
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=Path("build/tests/host_report.xml"))
    args = parser.parse_args()

    suite = ET.Element("testsuite", name="Host C tests", tests="4")
    failures = 0
    for name, script in (
        ("button_engine", "tests/button_engine/run.sh"),
        ("terminal_parser", "tests/terminal/run.sh"),
        ("input_manager", "tests/input_manager/run.sh"),
        ("command_dispatcher", "tests/dispatcher/run.sh"),
    ):
        start = time.monotonic()
        result = subprocess.run(["sh", script], capture_output=True, text=True)
        case = ET.SubElement(suite, "testcase", classname="host", name=name,
                             time=f"{time.monotonic() - start:.3f}")
        if result.returncode:
            failures += 1
            ET.SubElement(case, "failure", message=f"Exit code {result.returncode}").text = (
                result.stdout + result.stderr
            )
        output = ET.SubElement(case, "system-out")
        output.text = result.stdout + result.stderr
        print(f"{'FAIL' if result.returncode else 'PASS'}: {name}", flush=True)
        if result.returncode:
            print(result.stdout + result.stderr, flush=True)

    suite.set("failures", str(failures))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    ET.ElementTree(suite).write(args.output, encoding="utf-8", xml_declaration=True)
    raise SystemExit(1 if failures else 0)


if __name__ == "__main__":
    main()
