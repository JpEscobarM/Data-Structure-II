import csv
import pandas as pd
import os
import struct

# Caminho base da pasta
base_path = os.path.join(".", "Projeto-Trabalho-I")

# Garante que a pasta existe
os.makedirs(base_path, exist_ok=True)

# Caminhos dos arquivos
arquivo_origem = os.path.join(base_path, "jewelry.csv")
arquivo_tratado = os.path.join(base_path, "jewelry_tratado.csv")
arquivo_associacao = os.path.join(base_path, "associacao.bin")
arquivo_produto = os.path.join(base_path, "produto.bin")
arquivo_pedido = os.path.join(base_path, "pedido.bin")

# ============================================================
#   PARTE 1 - TRATAMENTO DO CSV ORIGINAL
# ============================================================

colunas = [
    "data", "id_pedido", "id_produto", "sku_qtd", "categoria_id",
    "alias", "marca", "preco", "usuario_id", "genero",
    "cor", "material", "joia"
]

df = pd.read_csv(arquivo_origem, dtype=str, header=None, names=colunas)

# Corrige 'genero' vazio
df["genero"] = df["genero"].apply(lambda x: "u" if pd.isna(x) or str(x).strip() == "" else x)
# Corta hora da data
df["data"] = df["data"].str.slice(0, 10)
# Remove colunas não utilizadas
df = df.drop(df.columns[[3, 4, 6, 8]], axis=1)

df.to_csv(
    arquivo_tratado,
    index=False,
    quoting=csv.QUOTE_NONE,
    escapechar='\\',
    encoding='latin1'
)
print("Arquivo CSV tratado salvo com sucesso:")
print(f" - {arquivo_tratado}")

# ============================================================
#   PARTE 2 - CRIAÇÃO DOS ARQUIVOS BINÁRIOS
# ============================================================

df = pd.read_csv(arquivo_tratado, dtype=str)

# ----------- 1. associacao.bin -----------
formato_assoc = "<20s20s"
with open(arquivo_associacao, "wb") as f_assoc:
    for _, linha in df.iterrows():
        id_pedido = str(linha["id_pedido"]).encode("latin1").ljust(20, b'\x00')
        id_produto = str(linha["id_produto"]).encode("latin1").ljust(20, b'\x00')
        f_assoc.write(struct.pack(formato_assoc, id_pedido, id_produto))
print(f"Arquivo BIN gerado: {arquivo_associacao}")

# ----------- 2. produto.bin (completo) -----------
df_produtos = df.drop_duplicates(subset=["id_produto"])
formato_prod = "<20s21s10s2s11s11s11s"

with open(arquivo_produto, "wb") as f_prod:
    for _, linha in df_produtos.iterrows():
        id_produto = str(linha["id_produto"]).encode("latin1").ljust(20, b'\x00')
        alias = str(linha["alias"] if pd.notna(linha["alias"]) else "").encode("latin1").ljust(21, b'\x00')
        preco = str(linha["preco"] if pd.notna(linha["preco"]) else "0.00").encode("latin1").ljust(10, b'\x00')
        genero = str(linha["genero"] if pd.notna(linha["genero"]) else "u").encode("latin1").ljust(2, b'\x00')
        cor = str(linha["cor"] if pd.notna(linha["cor"]) else "").encode("latin1").ljust(11, b'\x00')
        material = str(linha["material"] if pd.notna(linha["material"]) else "").encode("latin1").ljust(11, b'\x00')
        joia = str(linha["joia"] if pd.notna(linha["joia"]) else "").encode("latin1").ljust(11, b'\x00')
        f_prod.write(struct.pack(formato_prod, id_produto, alias, preco, genero, cor, material, joia))
print(f"Arquivo BIN gerado: {arquivo_produto}")

# ----------- 3. pedido.bin (resumido) -----------
# Campos: data(11s), id_pedido(20s), preco(10s)
formato_pedido = "<11s20s10s"

with open(arquivo_pedido, "wb") as f_pedido:
    for _, linha in df.iterrows():
        data = str(linha["data"]).encode("latin1").ljust(11, b'\x00')
        id_pedido = str(linha["id_pedido"]).encode("latin1").ljust(20, b'\x00')
        preco = str(linha["preco"] if pd.notna(linha["preco"]) else "0.00").encode("latin1").ljust(10, b'\x00')
        f_pedido.write(struct.pack(formato_pedido, data, id_pedido, preco))

print(f"Arquivo BIN gerado (pedido): {arquivo_pedido}")


print(f"\nResumo:")
print(f" - Total de associações: {len(df)} registros (40 bytes cada)")
print(f" - Total de produtos únicos: {len(df_produtos)} registros (86 bytes cada)")
print(f" - Total de pedidos: {len(df)} registros (41 bytes cada)")
print(f" - Tamanho total de pedido.bin ≈ {len(df) * 41} bytes")
