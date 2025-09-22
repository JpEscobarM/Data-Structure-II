
void fundirFolhas(No *alvo, No *irmao, No *pai, int idxSeparador, int ladoDireito)
{
    // Copia as chaves do irmão para o nó alvo
    for (int i = 0; i < irmao->qtdChaves; i++)
    {
        alvo->chaves[alvo->qtdChaves++] = irmao->chaves[i];
    }

    // Atualiza o ponteiro "prox" da folha
    alvo->prox = irmao->prox;

    // Libera o nó irmão
    free(irmao);

    // Remove a chave separadora do pai
    for (int i = idxSeparador; i < pai->qtdChaves - 1; i++)
    {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->filhos[i + 1] = pai->filhos[i + 2];
    }

    pai->qtdChaves--;

    // Se o pai ficar com menos do que ORDEM chaves e não for raiz, deverá propagar
    // (Essa lógica pode ser chamada recursivamente se necessário)
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

    // [CASO III] Fusão (concatenação)
    removerChaveFolha(folha, chave);

    if (irmaoEsq && irmaoEsq->qtdChaves == ORDEM)
    {
        fundirFolhas(irmaoEsq, folha, pai, idxFilho - 1, 0);
        printf("\n[CASO III-A] Fusão com irmão esquerdo.\n");
    }
    else if (irmaoDir && irmaoDir->qtdChaves == ORDEM)
    {
        fundirFolhas(folha, irmaoDir, pai, idxFilho, 1);
        printf("\n[CASO III-B] Fusão com irmão direito.\n");
    }
}
