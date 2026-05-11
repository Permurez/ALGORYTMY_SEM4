"""
Generuje wykresy z pliku results.csv (wyjscie benchmarku).
Uruchom po wykonaniu programu C++: python3 plot.py

Wymaga: pandas, matplotlib
  pip install pandas matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

CSV_FILE = "results.csv"

if not os.path.exists(CSV_FILE):
    print(f"Blad: nie znaleziono pliku {CSV_FILE}")
    print("Najpierw skompiluj i uruchom program C++.")
    sys.exit(1)

df = pd.read_csv(CSV_FILE)

DENSITIES   = [25, 50, 75, 100]
REPRS       = ["list", "matrix"]
REPR_LABELS = {"list": "Lista sasiedztwa", "matrix": "Macierz sasiedztwa"}
DENS_COLORS = {25: "royalblue", 50: "darkorange", 75: "forestgreen", 100: "crimson"}
REPR_STYLES = {"list": ("o", "solid"), "matrix": ("s", "dashed")}

# ---------------------------------------------------------------------------
# Wykresy Typ 1: osobny wykres dla kazdej reprezentacji,
#                parametrem krzywej jest gestosc (4 linie)
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
fig.suptitle("Dijkstra – czas wykonania vs liczba wierzcholkow\n(Typ 1: osobno dla kazdej reprezentacji)", fontsize=13)

for ax, rep in zip(axes, REPRS):
    sub = df[df["representation"] == rep]
    for d in DENSITIES:
        data = sub[sub["density"] == d].sort_values("vertices")
        ax.plot(data["vertices"], data["avg_time_us"],
                marker="o", label=f"Gestosc {d}%", color=DENS_COLORS[d])
    ax.set_title(REPR_LABELS[rep])
    ax.set_xlabel("Liczba wierzcholkow V")
    ax.set_ylabel("Sredni czas [μs]")
    ax.legend(title="Gestosc")
    ax.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()
plt.savefig("chart_type1.png", dpi=150)
print("Zapisano: chart_type1.png")

# ---------------------------------------------------------------------------
# Wykresy Typ 2: osobny wykres dla kazdej gestosci,
#                parametrem krzywej jest reprezentacja (2 linie)
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(2, 2, figsize=(13, 9))
fig.suptitle("Dijkstra – czas wykonania vs liczba wierzcholkow\n(Typ 2: osobno dla kazdej gestosci)", fontsize=13)

for ax, d in zip(axes.flat, DENSITIES):
    for rep in REPRS:
        data = df[(df["density"] == d) & (df["representation"] == rep)].sort_values("vertices")
        marker, style = REPR_STYLES[rep]
        ax.plot(data["vertices"], data["avg_time_us"],
                marker=marker, linestyle=style, label=REPR_LABELS[rep])
    ax.set_title(f"Gestosc {d}%")
    ax.set_xlabel("Liczba wierzcholkow V")
    ax.set_ylabel("Sredni czas [μs]")
    ax.legend(title="Reprezentacja")
    ax.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()
plt.savefig("chart_type2.png", dpi=150)
print("Zapisano: chart_type2.png")

plt.show()
