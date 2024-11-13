# trunk-ignore-all(trivy/DS026)
# trunk-ignore-all(trivy/DS002)
# trunk-ignore-all(checkov/CKV_DOCKER_2)
# trunk-ignore-all(checkov/CKV_DOCKER_3)
FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# trunk-ignore(terrascan/AC_DOCKER_0002)
# trunk-ignore(hadolint/DL3008)
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    ca-certificates \
    git \
    wget \
    zip \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
