# Cuis vendor pin

- URL: https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev
- Commit: 2628e261acaf4577084f2e788e973d0269b44fc5
- License: image/vendor/cuis/LICENSE（upstream LICENSE のコピー。MIT）
- Upstream files: CuisImage/Cuis7.8.sources and CuisImage/Cuis7.9-8206.changes
- Extract input: CuisImage/Cuis7.8.sources then CuisImage/Cuis7.9-8206.changes (sources then changes). `ao extract-vendor` reads that single concatenation. The last definition wins, so the changes file overrides the sources file, and only the allowlist's last definition is written to image/vendor/cuis/<Class>.st.
