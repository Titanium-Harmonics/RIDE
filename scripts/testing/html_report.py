#!/usr/bin/env python3
"""Generate a self-contained HTML report from Zephyr Twister JUnit XML."""
import argparse
import datetime as dt
import html
import os
from pathlib import Path
import subprocess
import sys
import xml.etree.ElementTree as ET


def escape(value):
    return html.escape(str(value or ""), quote=True)


def git_value(*args):
    try:
        return subprocess.check_output(
            ["git", *args], stderr=subprocess.DEVNULL, text=True
        ).strip()
    except (OSError, subprocess.CalledProcessError):
        return "unknown"


def generate(xml_file, output):
    root = ET.parse(xml_file).getroot()
    suites = [root] if root.tag == "testsuite" else root.findall(".//testsuite")
    if not suites:
        raise ValueError("No JUnit testsuites found in " + str(xml_file))

    rows = []
    counts = {"passed": 0, "failed": 0, "errors": 0, "skipped": 0}
    for suite in suites:
        suite_name = suite.get("name", "Unnamed suite")
        for case in suite.findall("testcase"):
            failure = case.find("failure")
            error = case.find("error")
            skipped = case.find("skipped")
            if failure is not None:
                status, detail = "failed", failure
            elif error is not None:
                status, detail = "errors", error
            elif skipped is not None:
                status, detail = "skipped", skipped
            else:
                status, detail = "passed", None
            counts[status] += 1
            message = ""
            if detail is not None:
                message = (detail.get("message") or "") + "\n" + (detail.text or "")
            rows.append(
                "<tr><td>" + escape(suite_name) + "</td><td>"
                + escape(case.get("classname", "")) + "</td><td>"
                + escape(case.get("name", "")) + "</td><td><span class='"
                + status + "'>" + status.upper() + "</span></td><td>"
                + escape(case.get("time", "")) + "</td><td><pre>"
                + escape(message.strip()) + "</pre></td></tr>"
            )

    total = sum(counts.values())
    revision = os.environ.get("GITHUB_SHA") or git_value("rev-parse", "--short", "HEAD")
    generated = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    body = """<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>R.I.D.E. Firmware Test Report</title>
<style>
:root{color-scheme:light dark;font-family:system-ui,sans-serif}
body{max-width:1200px;margin:2rem auto;padding:0 1rem;line-height:1.5}
header{border-bottom:1px solid #8885;margin-bottom:1.5rem}
.summary{display:flex;gap:1rem;flex-wrap:wrap;margin:1.5rem 0}
.summary div{border:1px solid #8886;border-radius:8px;padding:.75rem 1.5rem;min-width:100px}
.summary strong{display:block;font-size:1.6rem}
table{width:100%;border-collapse:collapse}th,td{border-bottom:1px solid #8885;padding:.65rem;text-align:left;vertical-align:top}
th{position:sticky;top:0;background:Canvas}
.passed{color:#16803c}.failed,.errors{color:#d13d3d}.skipped{color:#a67918}
pre{white-space:pre-wrap;overflow-wrap:anywhere;max-width:480px;margin:0}
.table-wrap{overflow-x:auto}small{opacity:.75}
</style></head><body><header><h1>R.I.D.E. Firmware Test Report</h1>
<p>Twister / Ztest · native_sim · Revision: <code>REVISION</code> · Generated: GENERATED</p></header>
<section class="summary">SUMMARY</section>
<h2>Individual test cases</h2><div class="table-wrap"><table><thead><tr>
<th>Suite</th><th>Class</th><th>Test</th><th>Result</th><th>Seconds</th><th>Failure details</th>
</tr></thead><tbody>ROWS</tbody></table></div>
<p><small>Generated from JUnit XML. This report is static and works offline.</small></p></body></html>
"""
    cards = "".join(
        "<div><strong>" + str(value) + "</strong>" + label.title() + "</div>"
        for label, value in [("total", total), *counts.items()]
    )
    body = (body.replace("REVISION", escape(revision))
            .replace("GENERATED", escape(generated))
            .replace("SUMMARY", cards)
            .replace("ROWS", "".join(rows)))
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(body, encoding="utf-8")
    print("HTML report: " + str(output) + " (" + str(total) + " cases)")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xml", type=Path, default=Path("build/twister/twister_report.xml"))
    parser.add_argument("--output", type=Path, default=Path("build/reports/index.html"))
    args = parser.parse_args()
    if not args.xml.is_file():
        parser.error("JUnit XML not found: " + str(args.xml))
    try:
        generate(args.xml, args.output)
    except (ET.ParseError, ValueError) as exc:
        parser.error(str(exc))


if __name__ == "__main__":
    main()
