from typing import Dict, List


class ModelInterface:
    def preprocess(self, text: str) -> Dict[str, int]:
        pass

    def get_probabilities(self, token: int) -> Dict[int, float]:
        pass

    def postprocess(self, tokens: List[int]) -> str:
        pass
