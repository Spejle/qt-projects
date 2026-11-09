import tkinter as tk
from tkinter import ttk


def make_grid(parent, rows, cols, on_change):
    """Vytvori tabulku z Entry poli; on_change se zavola pri kazde zmene."""
    vars_ = []
    for r in range(rows):
        for c in range(cols):
            v = tk.StringVar()
            v.trace_add("write", lambda *_: on_change())
            ttk.Entry(parent, textvariable=v, width=8, justify="center").grid(
                row=r, column=c, padx=1, pady=1)
            vars_.append(v)
    return vars_


root = tk.Tk()
root.title("Tabulky")
nb = ttk.Notebook(root)
nb.pack(fill="both", expand=True, padx=8, pady=8)

# ---- A: pocet X v tabulce ----
tab_a = ttk.Frame(nb, padding=8)
nb.add(tab_a, text="A: počet X")
grid_a = ttk.Frame(tab_a)
grid_a.pack()
label_a = ttk.Label(tab_a, font=("TkDefaultFont", 12, "bold"))
label_a.pack(pady=(8, 0))


def update_a():
    count = sum(v.get().strip().lower() == "x" for v in vars_a)
    label_a.config(text=f"V tabulce jsou {count} X")


vars_a = make_grid(grid_a, 6, 5, update_a)
update_a()

# ---- B: soucet cisel ve sloupci ----
tab_b = ttk.Frame(nb, padding=8)
nb.add(tab_b, text="B: součet")
grid_b = ttk.Frame(tab_b)
grid_b.pack()
label_b = ttk.Label(tab_b, font=("TkDefaultFont", 12, "bold"))
label_b.pack(pady=(8, 0))


def update_b():
    total = 0.0
    for v in vars_b:
        try:
            total += float(v.get().replace(",", "."))
        except ValueError:
            pass  # prazdne nebo neciselne bunky se ignoruji
    label_b.config(text=f"Součet: {total:g}")


vars_b = make_grid(grid_b, 10, 1, update_b)
update_b()

root.mainloop()
