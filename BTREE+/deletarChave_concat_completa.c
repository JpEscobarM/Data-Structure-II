
void fundirFolhas(No *alvo, No *irmao, No *pai, int idxSeparador)
{
    for (int i = 0; i < irmao->qtdChaves; i++)
        alvo->chaves[alvo->qtdChaves++] = irmao->chaves[i];

    alvo->prox = irmao->prox;

    free(irmao);

    for (int i = idxSeparador; i < pai->qtdChaves - 1; i++)
    {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->filhos[i + 1] = pai->filhos[i + 2];
    }

    pai->qtdChaves--;
}

void fundirNosInternos(No *alvo, No *irmao, No *pai, int idxSeparador)
{
    // Desce a chave separadora do pai para o nó alvo
    alvo->chaves[alvo->qtdChaves++] = pai->chaves[idxSeparador];

    // Copia todas as chaves e filhos do irmão
    for (int i = 0; i < irmao->qtdChaves; i++)
        alvo->chaves[alvo->qtdChaves++] = irmao->chaves[i];

    for (int i = 0; i <= irmao->qtdChaves; i++)
    {
        alvo->filhos[alvo->qtdChaves - irmao->qtdChaves + i] = irmao->filhos[i];
        if (irmao->filhos[i]) irmao->filhos[i]->pai = alvo;
    }

    free(irmao);

    // Remove chave separadora do pai
    for (int i = idxSeparador; i < pai->qtdChaves - 1; i++)
    {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->filhos[i + 1] = pai->filhos[i + 2];
    }

    pai->qtdChaves--;
}

void deletarChave(Arvore *arv, int chave)
{
    No *folha = busca(arv->raiz, chave, 0);
    if (!folha)
    {
        printf("\n[ERRO] <deletarChave> Chave %d não encontrada na árvore.\n", chave);
        return;
    }

    int chaveEncontrada = verificaChave(folha, chave);

    if (!chaveEncontrada)
    {
        printf("\n[ERRO] <deletarChave> Chave %d não está no nó folha.\n", chave);
        return;
    }

    No *pai = folha->pai;
    if (!pai)
    {
        removerChaveFolha(folha, chave);

        if (folha->qtdChaves == 0)
        {
            free(folha);
            arv->raiz = NULL;
            printf("\n[CASO RAIZ-FOLHA] Chave %d removida. Árvore agora está vazia.\n", chave);
        }
        else
        {
            printf("\n[CASO RAIZ-FOLHA] Chave %d removida da raiz folha. Restam %d chaves.\n", chave, folha->qtdChaves);
        }
        return;
    }

    if (folha->qtdChaves > ORDEM)
    {
        removerChaveFolha(folha, chave);
        printf("\n[CASO I] Chave %d removida de folha com > ORDEM chaves.\n", chave);
        return;
    }

    int idxFilho = procuraIndiceFilho(folha, pai);
    No *irmaoEsq = (idxFilho > 0) ? pai->filhos[idxFilho - 1] : NULL;
    No *irmaoDir = (idxFilho < pai->qtdChaves) ? pai->filhos[idxFilho + 1] : NULL;

    if (irmaoEsq && irmaoEsq->qtdChaves > ORDEM)
    {
        removerChaveFolha(folha, chave);
        int chaveEmprestada = irmaoEsq->chaves[irmaoEsq->qtdChaves - 1];
        removerChaveFolha(irmaoEsq, chaveEmprestada);
        addChaveEmFolha(folha, chaveEmprestada);
        pai->chaves[idxFilho - 1] = folha->chaves[0];
        printf("\n[CASO II-B] Redistribuição com irmão esquerdo. Chave emprestada: %d\n", chaveEmprestada);
        return;
    }
    else if (irmaoDir && irmaoDir->qtdChaves > ORDEM)
    {
        removerChaveFolha(folha, chave);
        int chaveEmprestada = irmaoDir->chaves[0];
        removerChaveFolha(irmaoDir, chaveEmprestada);
        addChaveEmFolha(folha, chaveEmprestada);
        pai->chaves[idxFilho] = irmaoDir->chaves[0];
        printf("\n[CASO II-B] Redistribuição com irmão direito. Chave emprestada: %d\n", chaveEmprestada);
        return;
    }

    // Caso III: fusão
    removerChaveFolha(folha, chave);

    if (irmaoEsq && irmaoEsq->qtdChaves == ORDEM)
    {
        fundirFolhas(irmaoEsq, folha, pai, idxFilho - 1);
        printf("\n[CASO III-A] Fusão de folhas com irmão esquerdo.\n");
    }
    else if (irmaoDir && irmaoDir->qtdChaves == ORDEM)
    {
        fundirFolhas(folha, irmaoDir, pai, idxFilho);
        printf("\n[CASO III-B] Fusão de folhas com irmão direito.\n");
    }

    // Se o pai virou nó interno com qtdChaves < ORDEM
    if (pai->qtdChaves < ORDEM && pai->pai)
    {
        int idxPai = procuraIndiceFilho(pai, pai->pai);
        No *paiIrmaoEsq = (idxPai > 0) ? pai->pai->filhos[idxPai - 1] : NULL;
        No *paiIrmaoDir = (idxPai < pai->pai->qtdChaves) ? pai->pai->filhos[idxPai + 1] : NULL;

        if (paiIrmaoEsq && paiIrmaoEsq->qtdChaves == ORDEM)
        {
            fundirNosInternos(paiIrmaoEsq, pai, pai->pai, idxPai - 1);
            printf("\n[PROPAGAÇÃO] Fusão de nós internos com irmão esquerdo.\n");
        }
        else if (paiIrmaoDir && paiIrmaoDir->qtdChaves == ORDEM)
        {
            fundirNosInternos(pai, paiIrmaoDir, pai->pai, idxPai);
            printf("\n[PROPAGAÇÃO] Fusão de nós internos com irmão direito.\n");
        }
    }
}
