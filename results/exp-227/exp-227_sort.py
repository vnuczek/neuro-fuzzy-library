import pandas as pd

df = pd.read_csv("exp-227-results.csv", sep=";")

df_sorted = df.sort_values(by=["Dataset", "Missing Ratio", "Frobenius Value"])

df_sorted.to_csv("exp-227-results_sorted.csv", sep=";", index=False)
