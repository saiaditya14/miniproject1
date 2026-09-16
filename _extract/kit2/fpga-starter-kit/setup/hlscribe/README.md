# hlscribe — course distribution

This folder ships `hlscribe` as a prebuilt Python wheel so you do **not** need
to clone any repository. Install it into a Python 3.11+ virtual environment:

```bash
pip install hlscribe/hlscribe-0.1.0-py3-none-any.whl
hlscribe --version          # sanity check
```

The wheel pulls its dependencies (`click`, `rich`, `pyyaml`, `python-dotenv`,
`anthropic`, `openai`) from PyPI, so run the install on a machine with internet
(the RCE login node has it).

Full setup and usage: [`../hlscribe_setup.md`](../hlscribe_setup.md).
